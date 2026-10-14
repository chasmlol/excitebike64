/* The track editor (edit.c, arnedgfx.c) and user tracks (UsrTrk.c): the piece types, the editor state with its
   pieces, and a user track as it is packed, laid out and built for a race. */
#ifndef STRUCTS_EDITOR_H
#define STRUCTS_EDITOR_H

#include "common.h"
#include "structs/model.h"
#include "structs/resman.h"
#include "structs/track.h"
#include "structs/font.h"
#include "structs/gui.h"

/* The track grid: a cell is 8 m, 500 units of the pieces' vertices (16 mm), 125 of the wall meshes (64 mm) and
   8000 of the racing line and the editor's camera (mm). */
#define CELL_VTX_UNITS  500
#define CELL_WALL_UNITS 125
#define CELL_MM         8000

/* A piece's meshes: one cell of 500x500 units per grid cell, in rows of dim[0] cells. A PrimModel. */
typedef struct PieceMesh {
    /* 0x00 */ u32 vtxCount;
    /* 0x04 */ Vtx *vtx;
    /* 0x08 */ u32 triCount;
    /* 0x0C */ u32 *tris;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
} PieceMesh; /* size 0x18 */

/* A cell of a mesh set. The user track's sets become the race's TrackModelLists (gTrackSections, gTrackObjects):
   the fields are a TrackModel's. */
typedef struct PieceCell {
    /* 0x00 */ PieceMesh *mesh;
    /* 0x04 */ s32 x0;              /* bounds */
    /* 0x08 */ s32 y0;
    /* 0x0C */ s32 x1;
    /* 0x10 */ s32 y1;
    /* 0x14 */ s16 *grid;           /* the ground triangles, a CollGrid: {flags (1 valid, 4 vertices relative to the
                                       cell's centre), count, CollTri[count] (8x8 cell bits, vertices, flags)} */
    /* 0x18 */ s32 visible;
    /* 0x1C */ s32 mask;
} PieceCell; /* size 0x20 */

typedef struct PieceMeshSet {
    /* 0x00 */ u32 count;
    /* 0x04 */ PieceCell cells[1];
} PieceMeshSet;

/* PieceType.flags */
#define PIECE_START        0x0002 /* the track can start on it */
#define PIECE_TURN         0x0004 /* the four turns */
#define PIECE_IN_MENU      0x0008 /* listed in the piece menus */
#define PIECE_FITS         0x0010 /* fits at the open end (edit_update_fitting_pieces) */
#define PIECE_FITS_FLIPPED 0x0020
#define PIECE_USER         0x0040 /* User Piece 1-8, New User Piece */
#define PIECE_VAR_LENGTH   0x0080 /* New User Piece: as long as the room left */
#define PIECE_FLIPPABLE    0x0100 /* R flips it */
#define PIECE_SIZE_LOCKED  0x0200 /* a user piece with pieces on both sides: its length cannot change */
#define PIECE_USED_TWICE   0x0400 /* a user piece placed more than once */
#define PIECE_CORNER       0x0800 /* the height editor shows the pieces up to the next corner */

/* gPieceTypes: 0-1 straights, 2-5 turns, 6-17 jumps, 18-25 User Piece 1-8, 26 New User Piece, 27 empty ground,
   28 the user piece template (one segment of meshes that user pieces are cloned from). */
#define PIECE_TYPE_STRAIGHT   0
#define PIECE_TYPE_TURN       2
#define PIECE_TYPE_JUMP       6
#define PIECE_TYPE_USER       18
#define PIECE_TYPE_NEW_USER   26
#define PIECE_TYPE_GROUND     27
#define PIECE_TYPE_TEMPLATE   28
#define PIECE_TYPE_COUNT      29

/* The user pieces: up to 8 segments (2 cells each), with a height at each of the 2 * 8 - 1 points between the
   piece's ends (gUserPieceHeights). */
#define USER_PIECE_COUNT        8
#define USER_PIECE_MAX_SEGMENTS 8
#define USER_PIECE_HEIGHTS      15

/* Sides of the grid (PieceType.dir, EditPiece.dir): a piece's rot turns them. */
#define SIDE_POS_Y 0
#define SIDE_POS_X 1
#define SIDE_NEG_Y 2
#define SIDE_NEG_X 3

typedef struct PieceType {
    /* 0x00 */ s16 dim[2];          /* cells across and along; a user piece's dim[1] is its segment count */
    /* 0x04 */ s16 dir[2];          /* sides of the connectors: [0] entry, [1] exit */
    /* 0x08 */ s16 off[2];          /* connector positions along their sides */
    /* 0x0C */ s16 flags;           /* PIECE_* (signed as most of the code reads it) */
    /* 0x0E */ s16 cursorAngle;     /* direction of the cursor arrow, 1/8 turns */
    /* 0x10 */ s16 rot;             /* rotation of a first piece (quarter turns) */
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ char *name;
    /* 0x20 */ u32 *meshAId;        /* resource ids (pointers to the id words), NULL for none */
    /* 0x24 */ u32 *meshBId;
    /* 0x28 */ u32 *pathId;
    /* 0x2C */ u32 *collisionId;
    /* 0x30 */ PieceMeshSet *meshA; /* textured ground */
    /* 0x34 */ PieceMeshSet *meshB; /* untextured */
    /* 0x38 */ s32 *path;           /* {count, nodes of 0x2C bytes}: the piece's part of the racing line */
    /* 0x3C */ WallMesh *collision;
} PieceType; /* size 0x40 */

/* EditPiece.flags */
#define EDIT_PIECE_FLIPPED 2
#define EDIT_PIECE_PREVIEW 4        /* a piece on trial at the open end */
#define EDIT_PIECE_VISIBLE 8        /* shown in the 3D view (the height editor hides the pieces away from its piece) */

/* A placed piece, in a list from first to last (a closed circuit links last to first). */
typedef struct EditPiece {
    /* 0x00 */ s16 used;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ struct EditPiece *next;
    /* 0x08 */ struct EditPiece *prev;
    /* 0x0C */ s16 x;               /* grid cell */
    /* 0x0E */ s16 y;
    /* 0x10 */ u16 flags;           /* EDIT_PIECE_* */
    /* 0x12 */ s16 rot;             /* quarter turns */
    /* 0x14 */ PieceType *type;
    /* 0x18 */ s16 dir[2];          /* the connectors after rotation and flip: [0] toward prev, [1] toward next */
    /* 0x1C */ s16 off[2];
    /* 0x20 */ s16 w;               /* size after rotation */
    /* 0x22 */ s16 h;
} EditPiece; /* size 0x24 */

/* The editor's camera target or position: x and z in 1/1000 units, yaw in 1/65536 turns (<< 13: eighths). */
typedef struct EditCamPos {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 z;
    /* 0x08 */ u32 yaw;
} EditCamPos; /* size 0xC */

/* An orbit camera: distance, pitch and height (the track view and the user piece view). */
typedef struct EditCam {
    /* 0x00 */ s32 dist;
    /* 0x04 */ s32 pitch;
    /* 0x08 */ s32 height;
} EditCam; /* size 0xC */

/* Editor.page */
#define EDIT_PAGE_MAIN  0
#define EDIT_PAGE_TRACK 1

/* Editor.mode */
#define EDIT_MODE_IDLE          0
#define EDIT_MODE_MOVE_TRACK    4
#define EDIT_MODE_USER_PIECE    6   /* editing a user piece's heights */
#define EDIT_MODE_NAME          7   /* name entry */
#define EDIT_MODE_NAME_8        8   /* also name entry */
#define EDIT_MODE_PLACE         9   /* choosing the next piece */
#define EDIT_MODE_WARN_MULTI    10  /* "Warning": the user piece is used more than once */
#define EDIT_MODE_WARN_LOCKED   11  /* "Warning": the user piece's size is locked */

/* Editor.slideState: the menu panel */
#define EDIT_SLIDE_IN    0
#define EDIT_SLIDE_SHOWN 1
#define EDIT_SLIDE_OUT   2
#define EDIT_SLIDE_SWAP  3          /* out: show nextMenu */

/* Editor.dialog: the OK/Cancel boxes */
#define EDIT_ASK_ADD_SEGMENT    0   /* "Add Segment?" */
#define EDIT_ASK_DELETE_SEGMENT 1   /* "Delete Will Reset The Piece" */
#define EDIT_ASK_EXIT           3   /* "Lose Changes?" before leaving */
#define EDIT_ASK_CLEAR          4   /* "Really Clear Track?" */
#define EDIT_ASK_SAVE           5   /* "Save The Track To..." Game Pak / Controller Pak */
#define EDIT_ASK_LOAD           6   /* "Load A Track From..." Game Pak / Controller Pak */
#define EDIT_ASK_LOAD_DISCARD   7   /* "Lose Changes?" before loading */
#define EDIT_ASK_OVERWRITE      8   /* "Overwrite Track On Game Pak?" */

/* Editor.message: the message boxes */
#define EDIT_MSG_NO_TRACK       0   /* "No Track Found On Game Pak" */
#define EDIT_MSG_SAVE_CANCELED  1
#define EDIT_MSG_TRACK_COMPLETE 2
#define EDIT_MSG_OUT_OF_ROOM    3
#define EDIT_MSG_NAME_FIRST     4   /* "Name Your Track Before Saving!" */

/* The items of the main menu (sEditMainMenu) and the edit menu (sEditTrackMenu) */
#define EDIT_MAIN_CLEAR       0
#define EDIT_MAIN_EDIT        1
#define EDIT_MAIN_LOAD        2
#define EDIT_MAIN_SAVE        3
#define EDIT_MAIN_TEST        4
#define EDIT_MAIN_DIRT        5     /* "Switch Dirt Type" */
#define EDIT_MAIN_NAME        6
#define EDIT_MAIN_EXIT        7
#define EDIT_ITEM_STRAIGHT    0     /* "Add Straight": the value picks the type */
#define EDIT_ITEM_TURN        1
#define EDIT_ITEM_JUMP        2
#define EDIT_ITEM_USER        3
#define EDIT_ITEM_MOVE_TRACK  4
#define EDIT_ITEM_MOVE_START  5
#define EDIT_ITEM_DELETE      6
#define EDIT_ITEM_MAIN_MENU   7

/* Name entry: gKeyboardKeys is 5 rows of 8 keys, the cursor moves 100 units a column and 160 a row */
#define EDIT_NAME_LEN      10
#define EDIT_KEY_DEL       0x26
#define EDIT_KEY_OK        0x27
#define EDIT_KEYS_PER_ROW  8
#define EDIT_KEY_W         100
#define EDIT_KEY_H         160
#define EDIT_KEY_AREA      800      /* the cursor's range in x and y */

/* sEditViewRects */
#define EDIT_VIEW_SCREEN 0
#define EDIT_VIEW_TRACK  1          /* the 3D view */
#define EDIT_VIEW_MENU   2

#define EDIT_MAX_PIECES  150
#define EDIT_CAM_STEPS   35         /* camera moves are eased over this many frames */
#define EDIT_MAX_LINES   60         /* the height editor's lines per frame (Editor.lineVtx: 4 vertices each) */
#define EDIT_DIRT_TYPES  3

/* Editor.heightState: the colour of the selected height point */
#define EDIT_HEIGHT_AT_LIMIT   0
#define EDIT_HEIGHT_NEAR_LIMIT 1
#define EDIT_HEIGHT_FREE       2

/* Editor.flags */
#define EDIT_REVERSED       0x00000001 /* the view is turned around: left/right swap */
#define EDIT_TURN_LEFT      0x00000002 /* C buttons: turn the view around (toggles EDIT_REVERSED) */
#define EDIT_TURN_RIGHT     0x00000004
#define EDIT_FLIP           0x00000008 /* the current piece is flipped */
#define EDIT_EXIT           0x00000010
#define EDIT_ROOM_X_NEG     0x00000020 /* Move Track: room to move the track this way */
#define EDIT_ROOM_X_POS     0x00000040
#define EDIT_ROOM_Y_POS     0x00000080
#define EDIT_ROOM_Y_NEG     0x00000100
#define EDIT_OPEN_END       0x00000200 /* adding pieces after the last one */
#define EDIT_TEST_TRACK     0x00000400 /* leaving to race the track */
#define EDIT_MESSAGE        0x00000800 /* a message box is up (message) */
#define EDIT_DIALOG         0x00001000 /* an OK/Cancel box is up (dialog, dialogChoice) */
#define EDIT_PAK_LOAD       0x00002000 /* away to the Controller Pak screen to load a track */
#define EDIT_PAK_SAVE       0x00004000 /* ... to save the track */
#define EDIT_CAN_DELETE     0x00008000
#define EDIT_A_HELD         0x00010000
#define EDIT_ROOM_ANY       (EDIT_ROOM_X_NEG | EDIT_ROOM_X_POS | EDIT_ROOM_Y_POS | EDIT_ROOM_Y_NEG)

/* The editor's state, gEditor (allocated by edit_init). */
typedef struct Editor {
    /* 0x0000 */ u8 unk_0000[0x16C];
    /* 0x016C */ s32 page;          /* EDIT_PAGE_* */
    /* 0x0170 */ s32 mode;          /* EDIT_MODE_* */
    /* 0x0174 */ u32 flags;         /* EDIT_* */
    /* 0x0178 */ s32 exitTimer;     /* counts up to 0 after EDIT_EXIT */
    /* 0x017C */ EditPiece *first;
    /* 0x0180 */ EditPiece *last;
    /* 0x0184 */ EditPiece *cur;
    /* 0x0188 */ EditPiece *markStart; /* a range drawn highlighted */
    /* 0x018C */ EditPiece *markEnd;
    /* 0x0190 */ EditPiece *start;  /* where the race starts */
    /* 0x0194 */ EditPiece pieces[EDIT_MAX_PIECES];
    /* 0x16AC */ s16 *gridSize;     /* {w, h} in cells: gEditArena */
    /* 0x16B0 */ s16 moveFromX;     /* Move Track: where the first piece was (B puts it back) */
    /* 0x16B2 */ s16 moveFromY;
    /* 0x16B4 */ s16 unk_16B4;
    /* 0x16B6 */ s16 scrollX;       /* where a first piece goes, from the grid centre */
    /* 0x16B8 */ s16 scrollY;
    /* 0x16BA */ char name[11];
    /* 0x16C5 */ char nameBuf[11];  /* name entry */
    /* 0x16D0 */ s32 nameKey;       /* selected key of gKeyboardKeys (EDIT_KEY_DEL, EDIT_KEY_OK) */
    /* 0x16D4 */ s32 nameCursorX;
    /* 0x16D8 */ s32 nameCursorY;
    /* 0x16DC */ s32 nameLen;
    /* 0x16E0 */ GuiMenu *menu;
    /* 0x16E4 */ GuiMenu *nextMenu; /* shown when the slide is over */
    /* 0x16E8 */ u32 slideState;    /* the menu panel: EDIT_SLIDE_* */
    /* 0x16EC */ s32 slideTimer;
    /* 0x16F0 */ s32 slideOffset;
    /* 0x16F4 */ s32 userPieceCount;
    /* 0x16F8 */ s32 heightSel;     /* selected height point of the user piece (1 .. 2 * segments - 1) */
    /* 0x16FC */ s32 userPiece;     /* the user piece being edited (0-7) */
    /* 0x1700 */ s16 heightState;   /* EDIT_HEIGHT_* */
    /* 0x1702 */ s16 maxSegments;   /* the longest user piece that fits */
    /* 0x1704 */ u8 unk_1704[8];
    /* 0x170C */ void *res170C;     /* loaded from the "Pieces" resource set */
    /* 0x1710 */ TexSet *cursorTex; /* the cursor arrow */
    /* 0x1714 */ PrimModel *cursorModel;
    /* 0x1718 */ TextObj *bigText;  /* font text objects */
    /* 0x171C */ TextObj *text;
    /* 0x1720 */ ResSet *pieceRes;  /* the "Pieces" resource set */
    /* 0x1724 */ TexSet *objectTex; /* textures of the pieces' B meshes */
    /* 0x1728 */ TexSet *wallTex;   /* the arena walls */
    /* 0x172C */ EditCam cam[2];    /* track view, user piece view */
    /* 0x1744 */ EditPiece *lastCur; /* cur as of the last frame */
    /* 0x1748 */ EditPiece *prevCur; /* the one before it */
    /* 0x174C */ Vtx *lineVtx[2];   /* the height editor's lines, per frame buffer (EDIT_MAX_LINES quads) */
    /* 0x1754 */ s32 lineCount;
    /* 0x1758 */ s32 camZoomFrames; /* the user piece camera moves for these frames ... */
    /* 0x175C */ s32 camZoomStep;   /* ... by this / 50 each */
    /* 0x1760 */ f32 camMtx[4][4];
    /* 0x17A0 */ Vp screenVp;
    /* 0x17B0 */ PieceType *selType; /* the type chosen in the piece menu */
    /* 0x17B4 */ u8 unk_17B4[4];
    /* 0x17B8 */ Vp views[4];       /* sEditViewRects */
    /* 0x17F8 */ s32 dirtType;      /* 0 to EDIT_DIRT_TYPES - 1 */
    /* 0x17FC */ s16 dialog;        /* which OK/Cancel box: EDIT_ASK_* */
    /* 0x17FE */ s16 dialogChoice;  /* 1 the left answer */
    /* 0x1800 */ s16 message;       /* which message box: EDIT_MSG_* */
    /* 0x1802 */ u8 unk_1802[2];
    /* 0x1804 */ Pic *warnFrame;    /* gui pictures */
    /* 0x1808 */ Pic *warnIcon;
    /* 0x180C */ u8 unk_180C[4];
} Editor; /* size 0x1810 */

/* gEditArena: the editor's grid and the arena around it. */
typedef struct EditArena {
    /* 0x00 */ s16 w;               /* grid size in cells (Editor.gridSize points here) */
    /* 0x02 */ s16 h;
    /* 0x04 */ u32 *wallTexId;      /* resource ids (pointers to the id words) */
    /* 0x08 */ u32 *collisionId;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ WallMesh *collision; /* the arena walls, loaded for a race (usrtrk_load_pieces) */
} EditArena; /* size 0x14 */

/* A packed user track (usrtrk_pack_track): at most 0x310 bytes (SaveData.userTrack), in a 0x400-byte buffer
   (gGame.userTrack: after a 4-byte checksum for the Controller Pak). */
#define USER_TRACK_TAG      0x35    /* its first byte */
#define USER_TRACK_MAX_SIZE 0x310
#define USER_TRACK_BUF_SIZE 0x400

/* A user track as edit.c keeps it for a race (usrtrk_layout_from_editor) and as the race gets it
   (usrtrk_load_layout): the user pieces' heights, then the pieces from the start piece on. */
typedef struct UserTrackPiece {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ u16 flags;           /* EDIT_PIECE_* */
    /* 0x06 */ s16 rot;
    /* 0x08 */ s16 type;            /* index in gPieceTypes */
    /* 0x0A */ s16 unk_0A;
} UserTrackPiece; /* size 0xC */

typedef struct UserTrackLayout {
    /* 0x000 */ s32 pieceCount;
    /* 0x004 */ s32 userPieceCount;
    /* 0x008 */ s32 dirtType;
    /* 0x00C */ u8 segmentCounts[USER_PIECE_COUNT];
    /* 0x014 */ s16 heights[USER_PIECE_COUNT][USER_PIECE_HEIGHTS];
    /* 0x104 */ UserTrackPiece pieces[1]; /* pieceCount */
} UserTrackLayout;

/* A user track built for a race (usrtrk_load_pieces, usrtrk_build_track). */
typedef struct UserTrackData {
    /* 0x00 */ PieceMeshSet *meshA; /* one cell per grid cell */
    /* 0x04 */ TexSet *dirtTex;
    /* 0x08 */ PieceMeshSet *meshB;
    /* 0x0C */ TexSet *objectTex;
    /* 0x10 */ s16 *grids;          /* the cells' ground triangle grids (PieceCell.grid), rotated */
    /* 0x14 */ Vtx *vtxA;
    /* 0x18 */ PieceMesh *meshesA;
    /* 0x1C */ Vtx *vtxB;
    /* 0x20 */ PieceMesh *meshesB;
    /* 0x24 */ u32 *path;           /* a path resource for path_load (header 0xAA000003) */
    /* 0x28 */ WallMesh *collision; /* the arena's and the pieces' merged (usrtrk_build_track) */
} UserTrackData; /* size 0x2C */

#endif
