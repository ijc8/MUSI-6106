#include <cassert>
#include <cctype>
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
        if (p == 'K') {
            castleRights.whiteShort = true;
        } else if (p == 'Q') {
            castleRights.whiteLong = true;
        } else if (p == 'k') {
            castleRights.blackShort = true;
        } else if (p == 'q') {
            castleRights.blackLong = true;
        }
    }
    enPassant = ep == "-" ? std::nullopt : std::make_optional(Square(ep));
}

std::unordered_set<Move> GameState::generateMoves(Square src) const {
    // This generates all moves possible from a given square - including moves that may not be legal due to leaving the king in check.
    // TODO: Split this function up more.
    assert(getPieceAt(src).has_value());
    Piece piece = getPieceAt(src).value();
    std::unordered_set<Move> moves;

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

    if (piece.type == Piece::Type::King) {
        int delta[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        for (auto [dr, df] : delta) {
            moves.emplace(src, Square(src.rank + dr, src.file + df));
        }
    } else if (piece.type == Piece::Type::Knight) {
        int delta[][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        for (auto [dr, df] : delta) {
            moves.emplace(src, Square(src.rank + dr, src.file + df));
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
    // Filter out moves that go off the board or are already occupied by a piece of the same color.
    for (auto it = moves.begin(); it != moves.end();) {
        Square dst = it->dst;
        std::optional<Piece> capture;
        if (dst.rank > 7 || dst.file > 7 || ((capture = getPieceAt(dst)) && capture->color == turn)) {
            it = moves.erase(it);
        } else {
            it++;
        }
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

bool GameState::isLegal(Move move) const {
    std::optional<Piece> piece = getPieceAt(move.src);
    std::optional<Piece> capture = getPieceAt(move.dst);
    if (!piece.has_value() || piece->color != turn) {
        // Source square is empty or contains opponent's piece.
        return false;
    } else if (capture.has_value() && capture->color == turn) {
        // Destination square is blocked by our piece.
        return false;
    }

    if (move.promotion.has_value()) {
        // Must be a pawn.
        if (piece->type != Piece::Type::Pawn) return false;
        // Must be moving to the opponent's backrank.
        if (!((turn == Color::White && move.dst.rank == 7) || (turn == Color::Black && move.dst.rank == 0))) return false;
        // Can't promote to pawn or king.
        if (*move.promotion == Piece::Type::Pawn || *move.promotion == Piece::Type::King) return false;
    }

    // TODO: Handle castling.

    // Check if movement is valid for piece type.
    auto moves = generateMoves(move.src);
    if (moves.find(move) == moves.end()) {
        return false;
    }

    // Check if move leaves king in check.
    GameState copy(*this);
    copy.execute(move);
    if (copy.isCheck(turn)) {
        return false;
    }

    return true;
}

void GameState::execute(Move move) {
    // NOTE: This does not check `move` for legality.
    Piece piece = getPieceAt(move.src).value();
    setPieceAt(move.src, std::nullopt);
    if (move.promotion.has_value()) {
        setPieceAt(move.dst, Piece(move.promotion.value(), piece.color));
    } else {
        setPieceAt(move.dst, piece);
    }

    // Flip turn.
    turn = turn == Color::White ? Color::Black : Color::White;
    // Handle en passant.
    if (piece.type == Piece::Type::Pawn && abs(move.src.rank - move.dst.rank) == 2) {
        enPassant.emplace((move.src.rank + move.dst.rank) / 2, move.src.file);
    } else {
        enPassant.reset();
    }

    // TODO: Update move clocks, castling rights.
}

void Game::push(Move move) {
    // Push the move and a copy of the game state on to the history stack.
    history.emplace(move, GameState(*this));
    execute(move);
}

Move Game::pop() {
    auto [move, state] = history.top();
    history.pop();
    // TODO: Restore game state in a less hacky way.
    // (or, just store FEN strings in the history instead of GameStates.)
    setFen(state.getFen());
    return move;
}
