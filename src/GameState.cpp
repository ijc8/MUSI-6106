#include "GameState.h"
#include <array>
#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>

#include "GameState.h"

using namespace Chess;

// Starting position for a game of chess using standard rules.
const std::string Board::initialBoardFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
// Starting state for a game of chess using standard rules.
const std::string GameState::initialFen = Board::initialBoardFen + " w KQkq - 0 1";

const std::unordered_map<char, Piece::Type> Piece::FromChar{
        {'P', Piece::Type::Pawn},
        {'B', Piece::Type::Bishop},
        {'N', Piece::Type::Knight},
        {'R', Piece::Type::Rook},
        {'Q', Piece::Type::Queen},
        {'K', Piece::Type::King},
};

const std::unordered_map<Piece::Type, char> Piece::ToChar{
        {Piece::Type::Pawn, 'P'},
        {Piece::Type::Bishop, 'B'},
        {Piece::Type::Knight, 'N'},
        {Piece::Type::Rook, 'R'},
        {Piece::Type::Queen, 'Q'},
        {Piece::Type::King, 'K'},
};

Board::Board(const std::string &boardFen) {
    setBoardFen(boardFen);
}

std::string Board::getBoardFen() const {
    std::string fen;
    // Counts number of consecutive empty squares in a rank.
    int emptyCount = 0;
    // Helper function.
    auto flushEmpty = [&emptyCount, &fen]() {
        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
            emptyCount = 0;
        }
    };

    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            std::optional<Piece> piece = getPieceAt(Square(rank, file));
            if (piece.has_value()) {
                flushEmpty();
                char pieceChar = Piece::ToChar.at(piece.value().type);
                fen += (piece.value().color == Color::White ? pieceChar : tolower(pieceChar));
            } else {
                emptyCount++;
            }
        }
        flushEmpty();
        // Insert slash between ranks.
        if (rank > 0) {
            fen += '/';
        }
    }
    return fen;
}

void Board::setBoardFen(const std::string &boardFen) {
    int rank = 7, file = 0;
    for (const char c : boardFen) {
        assert(rank >= 0 && rank < 8 && file >= 0 && (file < 8 || c == '/'));
        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            int endFile = file + (c - '0');
            for (; file < endFile; file++) {
                setPieceAt(Square(rank, file), std::nullopt);
            }
        } else {
            setPieceAt(Square(rank, file), Piece(c));
            file++;
        }
    }
}

std::optional<Piece> Board::getPieceAt(Square square) const {
    assert(square.rank < 8 && square.file < 8);
    return board[square.rank][square.file];
}

void Board::setPieceAt(Square square, std::optional<Piece> piece) {
    board[square.rank][square.file] = piece;
    if (piece.has_value()) {
        pieceMap[square] = piece.value();
    } else {
        pieceMap.erase(square);
    }
}

std::unordered_map<Square, Piece> Board::getPieceMap() const {
    return pieceMap;
}

std::unordered_set<Square> Board::getPieces(Piece piece) const {
    std::unordered_set<Square> squares;
    for (const auto [square, candidate] : pieceMap) {
        if (candidate == piece) {
            squares.insert(square);
        }
    }
    return squares;
}


GameState::GameState(const std::string &fen) {
    setFen(fen);
}

std::string GameState::getFen() const {
    std::string boardFen = getBoardFen();
    char turnc = turn == Color::White ? 'w' : 'b';
    std::string castleFen = "";
    for (char p : std::string("KQkq")) {
        if (canCastle(Piece(p))) {
            castleFen += p;
        }
    }
    if (castleFen.empty()) {
        castleFen = '-';
    }
    std::string ep = enPassant.has_value() ? enPassant.value().toString() : "-";
    std::string halfmove = std::to_string(halfmoveClock);
    std::string fullmove = std::to_string(fullmoveNumber);
    return boardFen + " " + turnc + " " + castleFen + " " + ep + " " + halfmove + " " + fullmove;
}

void GameState::setFen(const std::string &fen) {
    std::istringstream stream(fen);
    std::string boardFen, castleFen, ep;
    char turnc;
    stream >> boardFen >> turnc >> castleFen >> ep >> halfmoveClock >> fullmoveNumber;
    setBoardFen(boardFen);
    turn = turnc == 'w' ? Color::White : Color::Black;
    for (char p : castleFen) {
        PlayerCastleRights &rights = (isupper(p) ? castleRights.white : castleRights.black);
        if (toupper(p) == 'K') {
            rights.kingSide = true;
        } else if (toupper(p) == 'Q') {
            rights.queenSide = true;
        }
    }
    enPassant = ep == "-" ? std::nullopt : std::make_optional(Square(ep));
}

std::unordered_set<Move> GameState::generateMoves(Square src) const {
    // This generates all moves possible from a given square - including moves that may not be legal due to leaving the king in check.
    // TODO: Split this function up more.
    std::unordered_set<Move> moves;
    if (!getPieceAt(src)) {
        return moves;  // No piece to move!
    }

    Piece piece = getPieceAt(src).value();
    if (piece.color != turn) {
        return moves;  // Not our piece to move!
    }

    auto generateSlidingMoves = [=, &moves](std::vector<std::array<int, 2>> &delta) {
        for (auto [dr, df] : delta) {
            Square dst = src;
            dst.rank += dr;
            dst.file += df;
            // NOTE: The `< 0` cases are handled automatically due to overflow.
            while (dst.rank <= 7 && dst.file <= 7) {
                std::optional<Piece> capture = getPieceAt(dst);
                if (capture.has_value()) {
                    if (capture->color != piece.color) {
                        moves.emplace(src, dst);
                    }
                    break;
                }
                moves.emplace(src, dst);
                dst.rank += dr;
                dst.file += df;
            }
        }
    };

    auto canCastle = [=](std::initializer_list<const char *> squares) {
        // Do the cheap test (piece blocking the castle) first.
        for (auto square : squares) {
            if (getPieceAt(Square(square))) {
                return false;
            }
        }
        // Then do the expensive test (some intermediate square would involve check).
        for (auto square : squares) {
            if (wouldBeInCheck(Move(src, Square(square)))) {
                return false;
            }
        }
        return true;
    };

    if (piece.type == Piece::Type::King) {
        int delta[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        for (auto [dr, df] : delta) {
            Square dst(src.rank + dr, src.file + df);
            std::optional<Piece> capture;
            if (dst.rank < 8 && dst.file < 8 && !((capture = getPieceAt(dst)) && capture->color == turn)) {
                moves.emplace(src, dst);
            }
        }
        // Handle castling.
        if (src == Square(turn == Color::White ? "e1" : "e8")) {
            if (turn == Color::White) {
                if (castleRights.white.kingSide && canCastle({"f1", "g1"})) {
                    moves.emplace(src, Square("g1"));
                }
                if (castleRights.white.queenSide && canCastle({"c1", "d1"})) {
                    moves.emplace(src, Square("c1"));
                }
            } else {
                if (castleRights.black.kingSide && canCastle({"f8", "g8"})) {
                    moves.emplace(src, Square("g8"));
                }
                if (castleRights.black.queenSide && canCastle({"c8", "d8"})) {
                    moves.emplace(src, Square("c8"));
                }
            }
        }
    } else if (piece.type == Piece::Type::Knight) {
        int delta[][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        for (auto [dr, df] : delta) {
            Square dst(src.rank + dr, src.file + df);
            std::optional<Piece> capture;
            if (dst.rank < 8 && dst.file < 8 && !((capture = getPieceAt(dst)) && capture->color == turn)) {
                moves.emplace(src, dst);
            }
        }
    } else if (piece.type == Piece::Type::Pawn) {
        int dr = piece.color == Color::White ? 1 : -1;
        std::unordered_set<Move> pawnMoves;
        {
            Square dst(src.rank + dr, src.file);
            if (!getPieceAt(dst).has_value()) {
                // Pawn isn't blocked, can push it up the board.
                pawnMoves.emplace(src, dst);
                if ((piece.color == Color::White && src.rank == 1) ||
                    (piece.color == Color::Black && src.rank == 6)) {
                    // Pawns can initially forward 2 squares (if there's nothing in the way.)
                    Square dst2(src.rank + dr * 2, src.file);
                    if (!getPieceAt(dst2).has_value()) {
                        // No possibility of promotion on first move, so we just add this directly to `moves`.
                        moves.emplace(src, dst2);
                    }
                }
            }
        }
        for (int df : {-1, 1}) {
            // Check for pawn captures (including en passant!).
            Square dst(src.rank + dr, src.file + df);
            if (dst.rank > 7 || dst.file > 7) continue;
            std::optional<Piece> capture = getPieceAt(dst);
            if ((enPassant.has_value() && *enPassant == dst) || (capture.has_value() && capture->color != turn)) {
                pawnMoves.emplace(src, dst);
            }
        }
        // Generate all possible promotions if pawn has reached back rank.
        for (auto move : pawnMoves) {
            if ((piece.color == Color::White && move.dst.rank == 7) ||
                (piece.color == Color::Black && move.dst.rank == 0)) {
                for (auto promotion : {Piece::Type::Knight, Piece::Type::Bishop, Piece::Type::Rook, Piece::Type::Queen}) {
                    moves.emplace(move.src, move.dst, promotion);
                }
            } else {
                moves.insert(move);
            }
        }
    } else if (piece.type == Piece::Type::Rook) {
        std::vector<std::array<int, 2>> delta{{-1, 0}, {0, -1}, {0, 1}, {1, 0}};
        generateSlidingMoves(delta);
    } else if (piece.type == Piece::Type::Bishop) {
        std::vector<std::array<int, 2>> delta{{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        generateSlidingMoves(delta);
    } else if (piece.type == Piece::Type::Queen) {
        std::vector<std::array<int, 2>> delta{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        generateSlidingMoves(delta);
    }
    return moves;
}

bool GameState::isCheck(Color color) const {
    Square kingPos = *getPieces(Piece(Piece::Type::King, color)).begin();
    for (const auto [square, piece] : pieceMap) {
        // If an opposing piece could capture our king, we're in check.
        if (piece.color == color) continue;
        for (auto move : generateMoves(square)) {
            if (move.dst == kingPos) {
                return true;
            }
        }
    }
    return false;
}

bool GameState::wouldBeInCheck(Move move) const {
    GameState copy(*this);
    copy.execute(move);
    return copy.isCheck(turn);
}

std::optional<std::optional<Color>> GameState::getOutcome() const {
    // I swear this nested use of `optional` is reasonable!
    // We return nullopt if the game is not yet over (no outcome).
    // Otherwise, we return the winner: White, Black, or (nested) nullopt for a stalemate.
    // TODO:
    // If we're in check, see if there's a way to get out. (If not, our opponent wins.)
    // If we're not in check, see if there's any move that doesn't put us in check. (If not, it's stalemate.)
    return std::nullopt;
}

bool GameState::isLegal(Move move) const {
    // Check if this is a valid motion for the piece at the source (ignoring check).
    auto moves = generateMoves(move.src);
    if (moves.find(move) == moves.end()) {
        return false;
    }

    // Check if the move leaves our king in check.
    if (wouldBeInCheck(move)) {
        return false;
    }

    return true;
}

void GameState::execute(Move move) {
    // NOTE: This does not check `move` for legality. You can do that beforehand with `isLegal()`.
    Piece piece = getPieceAt(move.src).value();
    std::optional<Piece> capture = getPieceAt(move.dst);
    setPieceAt(move.src, std::nullopt);
    if (move.promotion.has_value()) {
        setPieceAt(move.dst, Piece(move.promotion.value(), piece.color));
    } else {
        setPieceAt(move.dst, piece);
    }

    // Handle castling.
    if (piece.type == Piece::Type::King && abs(move.src.file - move.dst.file) > 1) {
        // We represent castling by the king's move to the rook's square.
        // We already moved the king, but we still need to move the rook to the other side of the king.
        if (move.dst.file > move.src.file) {
            setPieceAt(Square(move.dst.rank, 5), Piece(Piece::Type::Rook, piece.color));
            setPieceAt(Square(move.dst.rank, 7), std::nullopt);
        } else {
            setPieceAt(Square(move.dst.rank, 3), Piece(Piece::Type::Rook, piece.color));
            setPieceAt(Square(move.dst.rank, 0), std::nullopt);
        }
    }

    // Update castling rights.
    PlayerCastleRights &rights = (piece.color == Color::White) ? castleRights.white : castleRights.black;
    if (piece.type == Piece::Type::King) {
        // If the king moves, both castling rights are lost.
        rights.kingSide = false;
        rights.queenSide = false;
    } else if (piece.type == Piece::Type::Rook) {
        // If a rook moves, only rights for that rook's side are lost.
        if (move.src.rank == (piece.color == Color::White ? 0 : 7)) {
            if (move.src.rank == 0) {
                rights.queenSide = false;
            } else if (move.src.rank == 7) {
                rights.kingSide = false;
            }
        }
    } else if (capture && capture->type == Piece::Type::Rook) {
        // For completeness, update the castle rights when a rook is captured.
        PlayerCastleRights &opRights = (capture->color == Color::White) ? castleRights.white : castleRights.black;
        if (move.dst.rank == (capture->color == Color::White ? 0 : 7)) {
            if (move.dst.rank == 0) {
                opRights.queenSide = false;
            } else if (move.dst.rank == 7) {
                opRights.kingSide = false;
            }
        }
    }

    // Flip turn.
    turn = turn == Color::White ? Color::Black : Color::White;
    // Handle en passant.
    if (piece.type == Piece::Type::Pawn && abs(move.src.rank - move.dst.rank) == 2) {
        enPassant.emplace((move.src.rank + move.dst.rank) / 2, move.src.file);
    } else {
        enPassant.reset();
    }

    // TODO: Update move clocks.
}

void GameState::print() {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << "12345678"[rank] << "|";
        for (int file = 0; file < 8; file++) {
            std::optional<Piece> p = getPieceAt(Square(rank, file));
            std::cout << (p.has_value() ? p->toChar() : ' ') << "|";
        }
        std::cout << std::endl;
    }
    std::cout << "  ";
    for (int file = 0; file < 8; file++) {
        std::cout << (char)('a' + file) << " ";
    }
    std::cout << std::endl;
    std::cout << "FEN: " << getFen() << std::endl;
}

void Game::push(Move move) {
    // Push the move and a copy of the game state on to the history stack.
    history.emplace(move, GameState(*this));
    execute(move);
}

std::optional<Move> Game::peek() const {
    if (history.empty()) return std::nullopt;
    return std::get<0>(history.top());
}

std::optional<Move> Game::pop() {
    if (history.empty()) return std::nullopt;
    auto [move, state] = history.top();
    history.pop();
    // TODO: Restore game state in a less hacky way.
    // (or, just store FEN strings in the history instead of GameStates.)
    setFen(state.getFen());
    return move;
}

std::unordered_map<Square, std::optional<Piece>> GameState::getThreats() {
    GameState copy(*this);
    auto pieceMap = copy.getPieceMap();

    Chess::Color color = copy.getTurn();
    copy.turn = color == Color::White? Chess::Color::Black : Color::White;
    std::unordered_map<Square, std::optional<Piece>> threats;

    for (const auto [square, piece] : pieceMap) {
        // Iterating over the pieces of the same color and checking for legal moves
        if (piece.color != copy.turn) continue;

        for (auto move : copy.generateMoves(square)) {
            if (copy.getPieceAt(move.dst).has_value()) {
                threats[move.dst] = copy.getPieceAt(move.dst);
            }
        }
    }

    return threats;
}

std::unordered_map<Square, std::optional<Piece>> GameState::getAttackers() {


    Chess::Color color = getTurn();
    auto pieceMap = getPieceMap();
    std::unordered_map<Square, std::optional<Piece>> attackers;

    for (const auto [square, piece] : pieceMap) {
        // Iterating over the pieces of the same color and checking for legal moves
        if (piece.color != color) continue;

        for (auto move : generateMoves(square)) {
            if (getPieceAt(move.dst).has_value()) {
                attackers[square] = piece;
            }
        }
    }
    return attackers;
}
