# MUSI-6106
♘ Musical Chess ♞ (for Audio Software Engineering 🚀)

## Build Instructions

First, clone the repository and initialize submodules: e.g. `git clone --recurse-submodules -j8 https://github.com/ijc8/MUSI-6106.git`.

Run CMake to generate a project file for your preferred build tool, and build (e.g. `cmake . && make`).

Finally, run the "MusicalChess" target.

### Dependencies

- [JUCE](https://juce.com/) - included as git submodule (GPL license)
- [Catch2](https://github.com/catchorg/Catch2/tree/v2.x) - header-only library included directly in repository (BSL-1.0 license)
- [Subprocess](https://github.com/sheredom/subprocess.h) - header-only library included directly in repo (Unlicense license)
- Stockfish (or another chess engine) - optional dependency for playing against the computer; download binary from [the website](https://stockfishchess.org/)

Note that `CMakeLists.txt` is configured to compile with C++17.

Some third-party assets are included in `assets/`; see READMEs within subdirectories for details.

## Motivation
The main motivation for this project is to take a complex, interesting game and bring it into sound — with the aim of having the sonic output capture some of the interest of the game, and add something to the experience of playing it. The project will take the form of a system that generates audio based on input from a chess game. The inputs will be the board state (with move history) and derived data, such as relative positions of pieces, chess engine evaluation of game state, etc. We would like to explore multiple approaches to come up with creative ways to translate the happenings on the board to sound. We envision our final deliverable being a standalone application that will enable a user to play a game of chess against the computer (or another human) with real-time audio output based on the game.

## Use Cases, Target Users
The target audience for this application will be those with an interest in either response-driven generative music or chess — those with an interest in both will likely find the most enjoyment. Users can engage with this application through two primary use cases. First, they can use this to heighten the experience of playing or watching chess. Having musical cues uniquely tailored to the state of the game will create a more immersive and engaging environment. Additionally, learning how the application translates instances of the board to the sonification engine can provide the user with helpful information. For example, the level of dissonance being produced could clue the user into how likely a piece is to be captured. Second, with much practice and understanding, a user can use this as a pseudo-synthesizer, using the chessboard as a musical canvas, purposely crafting game states to produce interesting assortments of sound.

## Functionality, Differentiation
Users will interact with the application as if they were engaged in a typical game of chess. The GUI will feature an 8x8 checkerboard with the standard assortment of chess pieces arranged on the board. Basic information such as elapsed time, captured pieces, current turn status, and end of game status will also be displayed. The user will be able to move pieces via drag-and-drop, and perhaps through a series of single clicks. The user can further tailor the experience by interchanging chess engines and modifying some sonification settings on a high level — these features will likely be accessible through a menu screen.
This application differentiates itself from similar products via its combination of a real-time component, audio synthesis focus, and small scale. Other projects — such as [chessMusic](https://github.com/acarabott/chessMusic) by Arthur Carabott and [immortal-game](https://github.com/joelamb/immortal-game) by Joe Lamb, which are both on GitHub — can only be used with completed games and use sampled sounds for the sonification. The project [Chess Re-imagined](https://artscilab.atec.io/blog/chess-re-imagined) seems to have the closest resemblance to our application as it also implements real-time sonification; however,  Chess Re-Imagined was a one-time public performance featuring live musicians, a physical electronic chessboard, and 3D visual projections. Our intention is to build a self-contained, reusable application.

## Implementation
Our project consists of five major components:
1. Game State: A representation of the state of a chess game. Other components will interact with this to read/update the state of the game, and to query whether a move is legal.
2. UI: A standalone JUCE application that displays the game state and (in interactive modes) allows the user to make moves with the mouse. This will also include a configuration menu (sonification controls, engine selection, file selection for loading a saved game, etc.).
3. Engine Communication: An implementation of the [Universal Chess Interface](https://en.wikipedia.org/wiki/Universal_Chess_Interface), to enable our application to communicate with a dedicated chess engine (a child process that also implements UCI, such as [Stockfish](https://stockfishchess.org/)).
4. Serialization: A mechanism for saving and loading chess games from strings (e.g. from files on disk or the user’s clipboard). This will support [Forsyth-Edwards Notation](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) (which describes game state, but not move history), [Portable Game Notation](https://en.wikipedia.org/wiki/Portable_Game_Notation) (which describes move history, but not timing), and timestamped PGN (as used by Lichess). If we have time, it may also support fetching live games from Lichess.
5. Sonification: This is the heart of the project, and where we anticipate most of the design effort and risk. We will define an abstract base class that takes the game state and fills audio buffers. We will implement several subclasses implementing this interface to experiment with different strategies for sonification. This will also involve implementing synthesis objects (oscillators, filters, envelopes, etc.) as needed to enable different sonification ideas.

Our project may interact with external programs and services such as chess engines (which execute as separate processes, communicating with an interface via UCI) and Lichess (which can stream in-progress games), but we do not anticipate any essential library dependencies other than JUCE. (Libraries such as the [THC Chess Library](https://github.com/billforsternz/thc-chess-library) or the [Synthesis Toolkit](https://github.com/thestk/stk) may be convenient for this project, but neither is essential.) For fetching live games from Lichess, we may need a WebSocket client library such as [websocketpp](https://github.com/zaphoyd/websocketpp) or [Boost.Beast](https://www.boost.org/doc/libs/1_78_0/libs/beast/doc/html/index.html).

This information is summarized in the figure below.

![Block Diagram](block_diagram.svg)

### Sonification Schemes
We anticipate that we will experiment with multiple approaches to sonification over the course of the semester; this is the creative part of our project. That said, broadly, our schemes will take the form of chess state in -> audio out. That is, each sonification scheme will continually generate audio. When the game state changes, the sonification scheme will be notified, and it will have access to the updated state. Our goal is for the output to be a) dependent on the chess game (but possibly a lossy representation) and b) musical.

To make this a bit more concrete, here are two schemes we have considered and plan to implement:
#### Audible Threats
In this scheme, the sonifier scans the boards for threats (all piece-pairs where one piece could capture another on the next turn). Each threat will be mapped to a pair of oscillators, with the rank (row) determing frequency, the file (column) determining panning, and the piece type determining timbre (e.g. one waveform for pawn, another for knight, ...) for each oscillator. When a move occurs, the sonifier will update the set of oscillators as necessary to reflect the new set of threats. The quality of this scheme depends in part on the frequency mapping (as there are eight ranks, a diatonic scale is an obvious choice) and timbre mapping. Regardless, we anticipate that the overall effect will be something like this: the sonifier is initially silent (because there are no threats in the starting position). The sound becomes denser and more complex as the game develops, the position sharpens, and threats occur across the board. Towards the end game, as the ranks thin out, the sound simplifies again, until culminating in a final drone for checkmate (which necessarily contains a threat) or potentially silence in case of a stalemate. Additionally, the way that the sound develops will reflect whether the game is more [open](https://en.wikipedia.org/wiki/Glossary_of_chess#open_game) or [closed](https://en.wikipedia.org/wiki/Glossary_of_chess#closed_game). The most basic implementation requires only oscillators, panning, and mixing.

#### Evaluation -> Consonance
In this scheme, the sonifier uses the evaluation (that is, who the engine thinks is winning, and by how much) reported by an external chess engine (e.g. Stockfish) to control a high-level parameter of a musical process. For example, the sonifier may be playing chords according to a transition graph, and the evaluation may influence whether it tends towards more consonant or dissonant chords. Or the sonifier may be generating rhythms with sampled drum sounds, and the evaluation influences the density (say, increasing the filled-in beats of a Euclidean rhythm) or number of layers. Note that the evaluation is on a 1D scale of "black winning" to "even game" to "white winning", but these schemes could take the absolute value of the evaluation to get a measure of how close the game is. For instance, the musical output might remain tense, precise, and minimal when the game is close, but loosen up or start careening if the game is a blow-out. In any case, this scheme requires communication with an engine (to provide evaluations, since that is out of our project's scope), audio event scheduling, and the ability to play samples from a file.

## Algorithmic References
As described above, our application will rely on standard formats for chess representation (FEN, PGN) and communication (UCI). Developing a chess engine is not in the scope of our project, so we will not require the algorithms commonly used in that domain. For sonification, we will likely rely on a variety of synthesis algorithms as needed for our aesthetic aims and sonification strategies. For these, we may look to the Synthesis Toolkit and Miller Puckette's [Theory and Technique of Electronic Music](http://msp.ucsd.edu/techniques/latest/book-html/) as algorithmic references.
