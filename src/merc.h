/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>

#include <unistd.h>
#include <stdbool.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#if !defined(FALSE)
#define FALSE     0
#endif

#if !defined(TRUE)
#define TRUE     1
#endif

#define DECLARE_DO_FUN(fun)    DO_FUN    fun
#define DECLARE_SPEC_FUN(fun)  SPEC_FUN  fun
#define DECLARE_SPELL_FUN(fun) SPELL_FUN fun

/* Structure types. */

typedef struct affect_data AFFECT_DATA;
typedef struct area_data AREA_DATA;
typedef struct ban_data BAN_DATA;
typedef struct char_data CHAR_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct extra_descr_data EXTRA_DESCR_DATA;
typedef struct help_data HELP_DATA;
typedef struct kill_data KILL_DATA;
typedef struct mob_index_data MOB_INDEX_DATA;
typedef struct note_data NOTE_DATA;
typedef struct obj_data OBJ_DATA;
typedef struct obj_index_data OBJ_INDEX_DATA;
typedef struct pc_data PC_DATA;
typedef struct reset_data RESET_DATA;
typedef struct room_index_data ROOM_INDEX_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct weather_data WEATHER_DATA;
typedef struct mob_prog_data MPROG_DATA;            // MOBprogram
typedef struct mob_prog_act_list MPROG_ACT_LIST;    // MOBprogram

/* Function types. */

typedef void DO_FUN (CHAR_DATA*, char*);
typedef bool SPEC_FUN (CHAR_DATA*);
typedef void SPELL_FUN (int, int, CHAR_DATA*, void*);

/* String and memory management parameters. */

#define MAX_KEY_HASH       1024
#define MAX_STRING_LENGTH  4096
#define MAX_INPUT_LENGTH    160

/* Game parameters.
Increase the max'es if you add more of something.
Adjust the pulse numbers to suit yourself. */

#define MAX_SKILL            90
#define MAX_CLASS             4
#define MAX_LEVEL            40
#define LEVEL_HERO           (MAX_LEVEL - 4)
#define LEVEL_IMMORTAL       (MAX_LEVEL - 3)

#define PULSE_PER_SECOND      4
#define PULSE_VIOLENCE       (3 * PULSE_PER_SECOND)
#define PULSE_MOBILE         (4 * PULSE_PER_SECOND)
#define PULSE_TICK           (30 * PULSE_PER_SECOND)
#define PULSE_AREA           (60 * PULSE_PER_SECOND)

/* Site ban structure. */

struct ban_data
{
    BAN_DATA *next;
    char *name;
};

/* Time and weather stuff. */

#define SUN_DARK        0
#define SUN_RISE        1
#define SUN_LIGHT       2
#define SUN_SET         3

#define SKY_CLOUDLESS   0
#define SKY_CLOUDY      1
#define SKY_RAINING     2
#define SKY_LIGHTNING   3

struct time_info_data
{
    int hour;
    int day;
    int month;
    int year;
};

struct weather_data
{
    int mmhg;
    int change;
    int sky;
    int sunlight;
};

/* Connected state for a channel. */

#define CON_PLAYING               0
#define CON_GET_NAME              1
#define CON_GET_OLD_PASSWORD      2
#define CON_CONFIRM_NEW_NAME      3
#define CON_GET_NEW_PASSWORD      4
#define CON_CONFIRM_NEW_PASSWORD  5
#define CON_GET_NEW_SEX           6
#define CON_GET_NEW_CLASS         7
#define CON_READ_MOTD             8

/* Descriptor (channel) structure. */

struct descriptor_data
{
    DESCRIPTOR_DATA *next;
    DESCRIPTOR_DATA *snoop_by;
    CHAR_DATA *character;
    CHAR_DATA *original;
    char *host;
    SOCKET descriptor;
    short connected;
    bool fcommand;
    char inbuf[4 * MAX_INPUT_LENGTH];
    char incomm[MAX_INPUT_LENGTH];
    char inlast[MAX_INPUT_LENGTH];
    int repeat;
    char *showstr_head;
    char *showstr_point;
    char *outbuf;
    int outsize;
    int outtop;
};

/* Attribute bonus structures. */

struct str_app_type
{
    short tohit;
    short todam;
    short carry;
    short wield;
};

struct int_app_type
{
    short learn;
};

struct wis_app_type
{
    short practice;
};

struct dex_app_type
{
    short defensive;
};

struct con_app_type
{
    short hitp;
    short shock;
};

/* TO types for act. */

#define TO_ROOM      0
#define TO_NOTVICT   1
#define TO_VICT      2
#define TO_CHAR      3

/* Help table types. */

struct help_data
{
    HELP_DATA *next;
    short level;
    char *keyword;
    char *text;
};

/* Shop types. */

#define MAX_TRADE    5

struct shop_data
{
    SHOP_DATA *next;                // Next shop in list
    short keeper;                   // Vnum of shop keeper mob
    short buy_type[MAX_TRADE];      // Item types shop will buy
    short profit_buy;               // Cost multiplier for buying
    short profit_sell;              // Cost multiplier for selling
    short open_hour;                // First opening hour
    short close_hour;               // First closing hour
};

/* Per-class stuff. */

struct class_type
{
    char who_name[4];   // Three-letter name for 'who'
    short attr_prime;   // Prime attribute
    short weapon;       // First weapon
    short guild;        // Vnum of guild room
    short skill_adept;  // Maximum skill level
    short thac0_00;     // Thac0 for level  0
    short thac0_32;     // Thac0 for level 32
    short hp_min;       // Min hp gained on leveling
    short hp_max;       // Max hp gained on leveling
    bool fMana;         // Class gains mana on level
};

/* Data structure for notes. */

struct note_data
{
    NOTE_DATA *next;
    char *sender;
    char *date;
    char *to_list;
    char *subject;
    char *text;
    time_t date_stamp;
};

/* An affect. */

struct affect_data
{
    AFFECT_DATA *next;
    short type;
    short duration;
    short location;
    short modifier;
    int bitvector;
};

/* A kill structure (indexed by level). */

struct kill_data
{
    short number;
    short killed;
};

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/* Well known mob virtual numbers.
Defined in #MOBILES. */

#define MOB_VNUM_CITYGUARD  3060
#define MOB_VNUM_VAMPIRE    3404

/* ACT bits for mobs.
Used in #MOBILES. */

#define ACT_IS_NPC         1  // Auto set for mobs
#define ACT_SENTINEL       2  // Stays in one room
#define ACT_SCAVENGER      4  // Picks up objects
#define ACT_AGGRESSIVE    32  // Attacks PC's
#define ACT_STAY_AREA     64  // Won't leave area
#define ACT_WIMPY        128  // Flees when hurt
#define ACT_PET          256  // Auto set for pets
#define ACT_TRAIN        512  // Can train PC's
#define ACT_PRACTICE    1024  // Can practice PC's

/* Bits for 'affected_by'.
Used in #MOBILES. */

#define AFF_BLIND                 1
#define AFF_INVISIBLE             2
#define AFF_DETECT_EVIL           4
#define AFF_DETECT_INVIS          8
#define AFF_DETECT_MAGIC         16
#define AFF_DETECT_HIDDEN        32
#define AFF_HOLD                 64  // Unused
#define AFF_SANCTUARY           128
#define AFF_FAERIE_FIRE         256
#define AFF_INFRARED            512
#define AFF_CURSE              1024
#define AFF_FLAMING            2048  // Unused
#define AFF_POISON             4096
#define AFF_PROTECT            8192
#define AFF_PARALYSIS         16384  // Unused
#define AFF_SNEAK             32768
#define AFF_HIDE              65536
#define AFF_SLEEP            131072
#define AFF_CHARM            262144
#define AFF_FLYING           524288
#define AFF_PASS_DOOR       1048576

/* Sex.
Used in #MOBILES. */

#define SEX_NEUTRAL     0
#define SEX_MALE        1
#define SEX_FEMALE      2

/* Well known object virtual numbers.
Defined in #OBJECTS. */

#define OBJ_VNUM_MONEY_ONE          2
#define OBJ_VNUM_MONEY_SOME         3

#define OBJ_VNUM_CORPSE_NPC        10
#define OBJ_VNUM_CORPSE_PC         11
#define OBJ_VNUM_SEVERED_HEAD      12
#define OBJ_VNUM_TORN_HEART        13
#define OBJ_VNUM_SLICED_ARM        14
#define OBJ_VNUM_SLICED_LEG        15
#define OBJ_VNUM_FINAL_TURD        16

#define OBJ_VNUM_MUSHROOM          20
#define OBJ_VNUM_LIGHT_BALL        21
#define OBJ_VNUM_SPRING            22

#define OBJ_VNUM_SCHOOL_MACE     3700
#define OBJ_VNUM_SCHOOL_DAGGER   3701
#define OBJ_VNUM_SCHOOL_SWORD    3702
#define OBJ_VNUM_SCHOOL_VEST     3703
#define OBJ_VNUM_SCHOOL_SHIELD   3704
#define OBJ_VNUM_SCHOOL_BANNER   3716

/* Item types.
Used in #OBJECTS. */

#define ITEM_LIGHT          1
#define ITEM_SCROLL         2
#define ITEM_WAND           3
#define ITEM_STAFF          4
#define ITEM_WEAPON         5
#define ITEM_TREASURE       8
#define ITEM_ARMOR          9
#define ITEM_POTION        10
#define ITEM_FURNITURE     12
#define ITEM_TRASH         13
#define ITEM_CONTAINER     15
#define ITEM_DRINK_CON     17
#define ITEM_KEY           18
#define ITEM_FOOD          19
#define ITEM_MONEY         20
#define ITEM_BOAT          22
#define ITEM_CORPSE_NPC    23
#define ITEM_CORPSE_PC     24
#define ITEM_FOUNTAIN      25
#define ITEM_PILL          26

/* Extra flags.
Used in #OBJECTS. */

#define ITEM_GLOW               1
#define ITEM_HUM                2
#define ITEM_DARK               4
#define ITEM_LOCK               8
#define ITEM_EVIL              16
#define ITEM_INVIS             32
#define ITEM_MAGIC             64
#define ITEM_NODROP           128
#define ITEM_BLESS            256
#define ITEM_ANTI_GOOD        512
#define ITEM_ANTI_EVIL       1024
#define ITEM_ANTI_NEUTRAL    2048
#define ITEM_NOREMOVE        4096
#define ITEM_INVENTORY       8192

/* Wear flags.
Used in #OBJECTS. */

#define ITEM_TAKE               1
#define ITEM_WEAR_FINGER        2
#define ITEM_WEAR_NECK          4
#define ITEM_WEAR_BODY          8
#define ITEM_WEAR_HEAD         16
#define ITEM_WEAR_LEGS         32
#define ITEM_WEAR_FEET         64
#define ITEM_WEAR_HANDS       128
#define ITEM_WEAR_ARMS        256
#define ITEM_WEAR_SHIELD      512
#define ITEM_WEAR_ABOUT      1024
#define ITEM_WEAR_WAIST      2048
#define ITEM_WEAR_WRIST      4096
#define ITEM_WIELD           8192
#define ITEM_HOLD           16384

/* Apply types (for affects).
Used in #OBJECTS. */

#define APPLY_NONE              0
#define APPLY_STR               1
#define APPLY_DEX               2
#define APPLY_INT               3
#define APPLY_WIS               4
#define APPLY_CON               5
#define APPLY_SEX               6
#define APPLY_CLASS             7
#define APPLY_LEVEL             8
#define APPLY_AGE               9
#define APPLY_HEIGHT           10
#define APPLY_WEIGHT           11
#define APPLY_MANA             12
#define APPLY_HIT              13
#define APPLY_MOVE             14
#define APPLY_GOLD             15
#define APPLY_EXP              16
#define APPLY_AC               17
#define APPLY_HITROLL          18
#define APPLY_DAMROLL          19
#define APPLY_SAVING_PARA      20
#define APPLY_SAVING_ROD       21
#define APPLY_SAVING_PETRI     22
#define APPLY_SAVING_BREATH    23
#define APPLY_SAVING_SPELL     24

/* Values for containers (value[1]).
Used in #OBJECTS. */

#define CONT_CLOSEABLE          1
#define CONT_PICKPROOF          2
#define CONT_CLOSED             4
#define CONT_LOCKED             8

/* Well known room virtual numbers.
Defined in #ROOMS. */

#define ROOM_VNUM_LIMBO         2
#define ROOM_VNUM_CHAT       1200
#define ROOM_VNUM_TEMPLE     3001
#define ROOM_VNUM_ALTAR      3054
#define ROOM_VNUM_SCHOOL     3700

/* Room flags.
Used in #ROOMS. */

#define ROOM_DARK               1
#define ROOM_NO_MOB             4
#define ROOM_INDOORS            8
#define ROOM_PRIVATE          512
#define ROOM_SAFE            1024
#define ROOM_SOLITARY        2048
#define ROOM_PET_SHOP        4096
#define ROOM_NO_RECALL       8192

/* Directions.
Used in #ROOMS. */

#define DIR_NORTH       0
#define DIR_EAST        1
#define DIR_SOUTH       2
#define DIR_WEST        3
#define DIR_UP          4
#define DIR_DOWN        5

/* Exit flags.
Used in #ROOMS. */

#define EX_ISDOOR       1
#define EX_CLOSED       2
#define EX_LOCKED       4
#define EX_PICKPROOF   32

/* Sector types.
Used in #ROOMS. */

#define SECT_INSIDE         0
#define SECT_CITY           1
#define SECT_FIELD          2
#define SECT_FOREST         3
#define SECT_HILLS          4
#define SECT_MOUNTAIN       5
#define SECT_WATER_SWIM     6
#define SECT_WATER_NOSWIM   7
#define SECT_UNUSED         8
#define SECT_AIR            9
#define SECT_DESERT        10
#define SECT_MAX           11

/* Equpiment wear locations.
Used in #RESETS. */

#define WEAR_NONE          -1
#define WEAR_LIGHT          0
#define WEAR_FINGER_L       1
#define WEAR_FINGER_R       2
#define WEAR_NECK_1         3
#define WEAR_NECK_2         4
#define WEAR_BODY           5
#define WEAR_HEAD           6
#define WEAR_LEGS           7
#define WEAR_FEET           8
#define WEAR_HANDS          9
#define WEAR_ARMS          10
#define WEAR_SHIELD        11
#define WEAR_ABOUT         12
#define WEAR_WAIST         13
#define WEAR_WRIST_L       14
#define WEAR_WRIST_R       15
#define WEAR_WIELD         16
#define WEAR_HOLD          17
#define MAX_WEAR           18

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/* Conditions. */

#define COND_DRUNK      0
#define COND_FULL       1
#define COND_THIRST     2

/* Positions. */

#define POS_DEAD        0
#define POS_MORTAL      1
#define POS_INCAP       2
#define POS_STUNNED     3
#define POS_SLEEPING    4
#define POS_RESTING     5
#define POS_FIGHTING    6
#define POS_STANDING    7

/* ACT bits for players. */

#define PLR_IS_NPC              1  // Don't EVER set.
#define PLR_BOUGHT_PET          2

#define PLR_AUTOEXIT            8
#define PLR_AUTOLOOT           16
#define PLR_AUTOSAC            32
#define PLR_BLANK              64
#define PLR_BRIEF             128
#define PLR_COMBINE           512
#define PLR_PROMPT           1024

//#define PLR_TELNET_GA        2048

#define PLR_HOLYLIGHT        4096
#define PLR_WIZINVIS         8192

#define    PLR_SILENCE      32768
#define PLR_NO_EMOTE        65536
#define PLR_NO_TELL        262144
#define PLR_LOG            524288
#define PLR_DENY          1048576
#define PLR_FREEZE        2097152
#define PLR_THIEF         4194304
#define PLR_KILLER        8388608

/* Obsolete bits. */

#if 0
#define PLR_AUCTION             4 // Obsolete
#define PLR_CHAT              256 // Obsolete
#define PLR_NO_SHOUT       131072 // Obsolete
#endif

/* Channel bits. */

#define    CHANNEL_AUCTION      1
#define    CHANNEL_CHAT         2
#define    CHANNEL_HACKER       4
#define    CHANNEL_IMMTALK      8
#define    CHANNEL_MUSIC       16
#define    CHANNEL_QUESTION    32
#define    CHANNEL_SHOUT       64
#define    CHANNEL_YELL       128

/* Prototype for a mob.
This is the in-memory version of #MOBILES. */

struct mob_index_data
{
    MOB_INDEX_DATA *next;
    SPEC_FUN *spec_fun;
    SHOP_DATA *pShop;
    char *player_name;
    char *short_descr;
    char *long_descr;
    char *description;
    short vnum;
    short count;
    short killed;
    short sex;
    short level;
    int act;
    int affected_by;
    short alignment;
    short hitroll;          // Unused
    short ac;               // Unused
    short hitnodice;        // Unused
    short hitsizedice;      // Unused
    short hitplus;          // Unused
    short damnodice;        // Unused
    short damsizedice;      // Unused
    short damplus;          // Unused
    int gold;               // Unused
    MPROG_DATA *mobprogs;   // Used by MOBprogram
    int progtypes;          // Used by MOBprogram
};

/* One character (PC or NPC). */

struct char_data
{
    CHAR_DATA *next;
    CHAR_DATA *next_in_room;
    CHAR_DATA *master;
    CHAR_DATA *leader;
    CHAR_DATA *fighting;
    CHAR_DATA *reply;
    SPEC_FUN *spec_fun;
    MOB_INDEX_DATA *pIndexData;
    DESCRIPTOR_DATA *desc;
    AFFECT_DATA *affected;
    NOTE_DATA *pnote;
    OBJ_DATA *carrying;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *was_in_room;
    PC_DATA *pcdata;
    char *name;
    char *short_descr;
    char *long_descr;
    char *description;
    char *prompt;
    short sex;
    short class;
    short race;
    short level;
    short trust;
    bool wizbit;
    int played;
    time_t logon;
    time_t save_time;
    time_t last_note;
    short timer;
    short wait;
    short hit;
    short max_hit;
    short mana;
    short max_mana;
    short move;
    short max_move;
    int gold;
    int exp;
    int act;
    int affected_by;
    short position;
    short practice;
    short carry_weight;
    short carry_number;
    short saving_throw;
    short alignment;
    short hitroll;
    short damroll;
    short armor;
    short wimpy;
    short deaf;
    MPROG_ACT_LIST *mpact;  // Used by MOBprogram
    int mpactnum;           // Used by MOBprogram
};

/* Data which only PC's have. */

struct pc_data
{
    PC_DATA *next;
    char *pwd;
    char *bamfin;
    char *bamfout;
    char *title;
    short perm_str;
    short perm_int;
    short perm_wis;
    short perm_dex;
    short perm_con;
    short mod_str;
    short mod_int;
    short mod_wis;
    short mod_dex;
    short mod_con;
    short condition[3];
    short pagelen;
    short learned[MAX_SKILL];
};

/* MOBprogram block */

struct mob_prog_act_list
{
    MPROG_ACT_LIST *next;
    char *buf;
    CHAR_DATA *ch;
    OBJ_DATA *obj;
    void *vo;
};

struct mob_prog_data
{
    MPROG_DATA *next;
    int type;
    char *arglist;
    char *comlist;
};

bool MOBtrigger;

#define ERROR_PROG         -1
#define IN_FILE_PROG        0
#define ACT_PROG            1
#define SPEECH_PROG         2
#define RAND_PROG           4
#define FIGHT_PROG          8
#define DEATH_PROG         16
#define HITPRCNT_PROG      32
#define ENTRY_PROG         64
#define GREET_PROG        128
#define ALL_GREET_PROG    256
#define GIVE_PROG         512
#define BRIBE_PROG       1024

/* Liquids. */

#define LIQ_WATER           0
#define LIQ_MAX            16

struct liq_type
{
    char *liq_name;
    char *liq_color;
    short liq_affect[3];
};

/* Extra description data for a room or object. */

struct extra_descr_data
{
    EXTRA_DESCR_DATA *next;   // Next in list
    char *keyword;            // Keyword in look/examine
    char *description;        // What to see
};

/* Prototype for an object. */

struct obj_index_data
{
    OBJ_INDEX_DATA *next;
    EXTRA_DESCR_DATA *extra_descr;
    AFFECT_DATA *affected;
    char *name;
    char *short_descr;
    char *description;
    short vnum;
    short item_type;
    short extra_flags;
    short wear_flags;
    short count;
    short weight;
    int cost;       // Unused
    int value[4];
};

/* One object. */

struct obj_data
{
    OBJ_DATA *next;
    OBJ_DATA *next_content;
    OBJ_DATA *contains;
    OBJ_DATA *in_obj;
    CHAR_DATA *carried_by;
    EXTRA_DESCR_DATA *extra_descr;
    AFFECT_DATA *affected;
    OBJ_INDEX_DATA *pIndexData;
    ROOM_INDEX_DATA *in_room;
    char *name;
    char *short_descr;
    char *description;
    short item_type;
    short extra_flags;
    short wear_flags;
    short wear_loc;
    short weight;
    int cost;
    short level;
    short timer;
    int value[4];
};

/* Exit data. */

struct exit_data
{
    ROOM_INDEX_DATA *to_room;
    short vnum;
    short exit_info;
    short key;
    char *keyword;
    char *description;
};

/* Reset commands:
    '*': comment
    'M': read a mobile 
    'O': read an object
    'P': put object in object
    'G': give object to mobile
    'E': equip object to mobile
    'D': set state of door
    'R': randomize room exits
    'S': stop (end of list) */

/* Area-reset definition. */

struct reset_data
{
    RESET_DATA *next;
    char command;
    short arg1;
    short arg2;
    short arg3;
};

/* Area definition. */

struct area_data
{
    AREA_DATA *next;
    RESET_DATA *reset_first;
    RESET_DATA *reset_last;
    char *name;
    short age;
    short nplayer;
};

/* Room type. */

struct room_index_data
{
    ROOM_INDEX_DATA *next;
    CHAR_DATA *people;
    OBJ_DATA *contents;
    EXTRA_DESCR_DATA *extra_descr;
    AREA_DATA *area;
    EXIT_DATA *exit[6];
    char *name;
    char *description;
    short vnum;
    short room_flags;
    short light;
    short sector_type;
};

/* Types of attacks.
Must be non-overlapping with spell/skill types,
but may be arbitrary beyond that. */

#define TYPE_UNDEFINED         -1
#define TYPE_HIT             1000

/* Target types. */

#define TAR_IGNORE              0
#define TAR_CHAR_OFFENSIVE      1
#define TAR_CHAR_DEFENSIVE      2
#define TAR_CHAR_SELF           3
#define TAR_OBJ_INV             4

/* Skills include spells as a particular case. */

struct skill_type
{
    char *name;                     // Name of skill
    short skill_level[MAX_CLASS];   // Level needed by class
    SPELL_FUN *spell_fun;           // Spell pointer (for spells)
    short target;                   // Legal targets
    short minimum_position;         // Position for caster / user
    short *pgsn;                    // Pointer to associated gsn
    short slot;                     // Slot for #OBJECT loading
    short min_mana;                 // Minimum mana used
    short beats;                    // Waiting time after use
    char *noun_damage;              // Damage message
    char *msg_off;                  // Wear off message
};

/* These are skill_lookup return values for common skills and spells. */

extern short gsn_backstab;
extern short gsn_dodge;
extern short gsn_hide;
extern short gsn_peek;
extern short gsn_pick_lock;
extern short gsn_sneak;
extern short gsn_steal;

extern short gsn_disarm;
extern short gsn_enhanced_damage;
extern short gsn_kick;
extern short gsn_parry;
extern short gsn_rescue;
extern short gsn_second_attack;
extern short gsn_third_attack;

extern short gsn_blindness;
extern short gsn_charm_person;
extern short gsn_curse;
extern short gsn_invis;
extern short gsn_mass_invis;
extern short gsn_poison;
extern short gsn_sleep;

/* Utility macros. */

#define UMIN(a, b)              ((a) < (b) ? (a) : (b))
#define UMAX(a, b)              ((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)         ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)                ((c) >= 'A' && (c) <= 'Z' ? (c) + 'a' - 'A' : (c))
#define UPPER(c)                ((c) >= 'a' && (c) <= 'z' ? (c) + 'A' - 'a' : (c))
#define IS_SET(flag, bit)       ((flag) & (bit))
#define SET_BIT(var, bit)       ((var) |= (bit))
#define REMOVE_BIT(var, bit)    ((var) &= ~(bit))

/* Character macros. */

#define IS_NPC(ch)              (IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)         (get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)             (get_trust(ch) >= LEVEL_HERO)
#define IS_AFFECTED(ch, sn)     (IS_SET((ch)->affected_by, (sn)))

#define IS_GOOD(ch)             (ch->alignment >= 350)
#define IS_EVIL(ch)             (ch->alignment <= -350)
#define IS_NEUTRAL(ch)          (!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)            (ch->position > POS_SLEEPING)

#define GET_AC(ch)              ((ch)->armor + (IS_AWAKE(ch) ? dex_app[ get_curr_dex(ch) ].defensive : 0))

#define GET_HITROLL(ch)         ((ch)->hitroll+str_app[ get_curr_str(ch) ].tohit)
#define GET_DAMROLL(ch)         ((ch)->damroll+str_app[ get_curr_str(ch) ].todam)

#define IS_OUTSIDE(ch)          (!IS_SET((ch)->in_room->room_flags, ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)  ((ch)->wait = UMAX((ch)->wait, (npulse)))

#define MANA_COST(ch, sn)       (IS_NPC(ch) ? 0 :   \
    UMAX (skill_table[ sn ].min_mana,               \
    100 / (2 + ch->level - skill_table[ sn ].skill_level[ ch->class ])))


/* Object macros. */

#define CAN_WEAR(obj, part)     (IS_SET((obj)->wear_flags, (part)))
#define IS_OBJ_STAT(obj, stat)  (IS_SET((obj)->extra_flags, (stat)))

/* Description macros. */

#define PERS(ch, looker)        \
    (can_see(looker, (ch)) ? (IS_NPC(ch) ? (ch)->short_descr : (ch)->name) : "someone")


/* Structure for a command in the command lookup table. */

struct cmd_type
{
    char *const name;
    DO_FUN *do_fun;
    short position;
    short level;
    short log;
};

/* Structure for a social in the socials table. */

struct social_type
{
    char *const name;
    char *const char_no_arg;
    char *const others_no_arg;
    char *const char_found;
    char *const others_found;
    char *const vict_found;
    char *const char_auto;
    char *const others_auto;
};

/* Global constants. */

extern const struct str_app_type str_app[26];
extern const struct int_app_type int_app[26];
extern const struct wis_app_type wis_app[26];
extern const struct dex_app_type dex_app[26];
extern const struct con_app_type con_app[26];

extern const struct class_type class_table[MAX_CLASS];
extern const struct cmd_type cmd_table[];
extern const struct liq_type liq_table[LIQ_MAX];
extern const struct skill_type skill_table[MAX_SKILL];
extern const struct social_type social_table[];
extern char *const title_table[MAX_CLASS][MAX_LEVEL + 1][2];

/* Global variables. */

extern HELP_DATA *help_first;
extern SHOP_DATA *shop_first;

extern BAN_DATA *ban_list;
extern CHAR_DATA *char_list;
extern DESCRIPTOR_DATA *descriptor_list;
extern NOTE_DATA *note_list;
extern OBJ_DATA *object_list;

extern AFFECT_DATA *affect_free;
extern BAN_DATA *ban_free;
extern CHAR_DATA *char_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern EXTRA_DESCR_DATA *extra_descr_free;
extern NOTE_DATA *note_free;
extern OBJ_DATA *obj_free;
extern PC_DATA *pcdata_free;

extern char bug_buf[];
extern time_t current_time;
extern bool fLogAll;
//extern FILE *fpReserve;
extern KILL_DATA kill_table[];
extern char log_buf[];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;

/* Command functions.
Defined in act_*.c (mostly). */

DECLARE_DO_FUN(do_advance);
DECLARE_DO_FUN(do_allow);
DECLARE_DO_FUN(do_answer);
DECLARE_DO_FUN(do_areas);
DECLARE_DO_FUN(do_at);
DECLARE_DO_FUN(do_auction);
DECLARE_DO_FUN(do_auto);
DECLARE_DO_FUN(do_autoexit);
DECLARE_DO_FUN(do_autoloot);
DECLARE_DO_FUN(do_autosac);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_bamfin);
DECLARE_DO_FUN(do_bamfout);
DECLARE_DO_FUN(do_ban);
DECLARE_DO_FUN(do_blank);
DECLARE_DO_FUN(do_brandish);
DECLARE_DO_FUN(do_brief);
DECLARE_DO_FUN(do_bug);
DECLARE_DO_FUN(do_buy);
DECLARE_DO_FUN(do_cast);
DECLARE_DO_FUN(do_channels);
DECLARE_DO_FUN(do_chat);
DECLARE_DO_FUN(do_close);
DECLARE_DO_FUN(do_combine);
DECLARE_DO_FUN(do_commands);
DECLARE_DO_FUN(do_compare);
DECLARE_DO_FUN(do_config);
DECLARE_DO_FUN(do_consider);
DECLARE_DO_FUN(do_credits);
DECLARE_DO_FUN(do_deny);
DECLARE_DO_FUN(do_description);
DECLARE_DO_FUN(do_disarm);
DECLARE_DO_FUN(do_disconnect);
DECLARE_DO_FUN(do_down);
DECLARE_DO_FUN(do_drink);
DECLARE_DO_FUN(do_drop);
DECLARE_DO_FUN(do_east);
DECLARE_DO_FUN(do_eat);
DECLARE_DO_FUN(do_echo);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_equipment);
DECLARE_DO_FUN(do_examine);
DECLARE_DO_FUN(do_exits);
DECLARE_DO_FUN(do_fill);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_follow);
DECLARE_DO_FUN(do_force);
DECLARE_DO_FUN(do_freeze);
DECLARE_DO_FUN(do_get);
DECLARE_DO_FUN(do_give);
DECLARE_DO_FUN(do_goto);
DECLARE_DO_FUN(do_group);
DECLARE_DO_FUN(do_gtell);
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_hide);
DECLARE_DO_FUN(do_holylight);
DECLARE_DO_FUN(do_idea);
DECLARE_DO_FUN(do_immtalk);
DECLARE_DO_FUN(do_inventory);
DECLARE_DO_FUN(do_invis);
DECLARE_DO_FUN(do_kick);
DECLARE_DO_FUN(do_kill);
DECLARE_DO_FUN(do_list);
DECLARE_DO_FUN(do_lock);
DECLARE_DO_FUN(do_log);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_memory);
DECLARE_DO_FUN(do_mfind);
DECLARE_DO_FUN(do_mload);
DECLARE_DO_FUN(do_mpasound);
DECLARE_DO_FUN(do_mpat);
DECLARE_DO_FUN(do_mpecho);
DECLARE_DO_FUN(do_mpechoaround);
DECLARE_DO_FUN(do_mpechoat);
DECLARE_DO_FUN(do_mpforce);
DECLARE_DO_FUN(do_mpgoto);
DECLARE_DO_FUN(do_mpjunk);
DECLARE_DO_FUN(do_mpkill);
DECLARE_DO_FUN(do_mpmload);
DECLARE_DO_FUN(do_mpoload);
DECLARE_DO_FUN(do_mppurge);
DECLARE_DO_FUN(do_mpstat);
DECLARE_DO_FUN(do_mptransfer);
DECLARE_DO_FUN(do_mset);
DECLARE_DO_FUN(do_mstat);
DECLARE_DO_FUN(do_mwhere);
DECLARE_DO_FUN(do_murde);
DECLARE_DO_FUN(do_murder);
DECLARE_DO_FUN(do_music);
DECLARE_DO_FUN(do_noemote);
DECLARE_DO_FUN(do_north);
DECLARE_DO_FUN(do_note);
DECLARE_DO_FUN(do_notell);
DECLARE_DO_FUN(do_ofind);
DECLARE_DO_FUN(do_oload);
DECLARE_DO_FUN(do_open);
DECLARE_DO_FUN(do_order);
DECLARE_DO_FUN(do_oset);
DECLARE_DO_FUN(do_ostat);
DECLARE_DO_FUN(do_owhere);
DECLARE_DO_FUN(do_pagelen);
DECLARE_DO_FUN(do_pardon);
DECLARE_DO_FUN(do_password);
DECLARE_DO_FUN(do_peace);
DECLARE_DO_FUN(do_pick);
DECLARE_DO_FUN(do_pose);
DECLARE_DO_FUN(do_practice);
DECLARE_DO_FUN(do_prompt);
DECLARE_DO_FUN(do_purge);
DECLARE_DO_FUN(do_put);
DECLARE_DO_FUN(do_quaff);
DECLARE_DO_FUN(do_question);
DECLARE_DO_FUN(do_qui);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_reboo);
DECLARE_DO_FUN(do_reboot);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_recho);
DECLARE_DO_FUN(do_recite);
DECLARE_DO_FUN(do_remove);
DECLARE_DO_FUN(do_rent);
DECLARE_DO_FUN(do_reply);
DECLARE_DO_FUN(do_report);
DECLARE_DO_FUN(do_rescue);
DECLARE_DO_FUN(do_rest);
DECLARE_DO_FUN(do_restore);
DECLARE_DO_FUN(do_return);
DECLARE_DO_FUN(do_rset);
DECLARE_DO_FUN(do_rstat);
DECLARE_DO_FUN(do_sacrifice);
DECLARE_DO_FUN(do_save);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_score);
DECLARE_DO_FUN(do_sell);
DECLARE_DO_FUN(do_shout);
DECLARE_DO_FUN(do_shutdow);
DECLARE_DO_FUN(do_shutdown);
DECLARE_DO_FUN(do_silence);
DECLARE_DO_FUN(do_sla);
DECLARE_DO_FUN(do_slay);
DECLARE_DO_FUN(do_sleep);
DECLARE_DO_FUN(do_slist);
DECLARE_DO_FUN(do_slookup);
DECLARE_DO_FUN(do_sneak);
DECLARE_DO_FUN(do_snoop);
DECLARE_DO_FUN(do_socials);
DECLARE_DO_FUN(do_south);
DECLARE_DO_FUN(do_spells);
DECLARE_DO_FUN(do_split);
DECLARE_DO_FUN(do_sset);
DECLARE_DO_FUN(do_stand);
DECLARE_DO_FUN(do_steal);
DECLARE_DO_FUN(do_switch);
DECLARE_DO_FUN(do_tell);
DECLARE_DO_FUN(do_time);
DECLARE_DO_FUN(do_title);
DECLARE_DO_FUN(do_train);
DECLARE_DO_FUN(do_transfer);
DECLARE_DO_FUN(do_trust);
DECLARE_DO_FUN(do_typo);
DECLARE_DO_FUN(do_unlock);
DECLARE_DO_FUN(do_up);
DECLARE_DO_FUN(do_users);
DECLARE_DO_FUN(do_value);
DECLARE_DO_FUN(do_visible);
DECLARE_DO_FUN(do_wake);
DECLARE_DO_FUN(do_wear);
DECLARE_DO_FUN(do_weather);
DECLARE_DO_FUN(do_west);
DECLARE_DO_FUN(do_where);
DECLARE_DO_FUN(do_who);
DECLARE_DO_FUN(do_wimpy);
DECLARE_DO_FUN(do_wizhelp);
DECLARE_DO_FUN(do_wizify);
DECLARE_DO_FUN(do_wizlist);
DECLARE_DO_FUN(do_wizlock);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_zap);

/* Spell functions.
Defined in magic.c. */

DECLARE_SPELL_FUN(spell_null);
DECLARE_SPELL_FUN(spell_acid_blast);
DECLARE_SPELL_FUN(spell_armor);
DECLARE_SPELL_FUN(spell_bless);
DECLARE_SPELL_FUN(spell_blindness);
DECLARE_SPELL_FUN(spell_burning_hands);
DECLARE_SPELL_FUN(spell_call_lightning);
DECLARE_SPELL_FUN(spell_cause_critical);
DECLARE_SPELL_FUN(spell_cause_light);
DECLARE_SPELL_FUN(spell_cause_serious);
DECLARE_SPELL_FUN(spell_change_sex);
DECLARE_SPELL_FUN(spell_charm_person);
DECLARE_SPELL_FUN(spell_chill_touch);
DECLARE_SPELL_FUN(spell_colour_spray);
DECLARE_SPELL_FUN(spell_continual_light);
DECLARE_SPELL_FUN(spell_control_weather);
DECLARE_SPELL_FUN(spell_create_food);
DECLARE_SPELL_FUN(spell_create_spring);
DECLARE_SPELL_FUN(spell_create_water);
DECLARE_SPELL_FUN(spell_cure_blindness);
DECLARE_SPELL_FUN(spell_cure_critical);
DECLARE_SPELL_FUN(spell_cure_light);
DECLARE_SPELL_FUN(spell_cure_poison);
DECLARE_SPELL_FUN(spell_cure_serious);
DECLARE_SPELL_FUN(spell_curse);
DECLARE_SPELL_FUN(spell_detect_evil);
DECLARE_SPELL_FUN(spell_detect_hidden);
DECLARE_SPELL_FUN(spell_detect_invis);
DECLARE_SPELL_FUN(spell_detect_magic);
DECLARE_SPELL_FUN(spell_detect_poison);
DECLARE_SPELL_FUN(spell_dispel_evil);
DECLARE_SPELL_FUN(spell_dispel_magic);
DECLARE_SPELL_FUN(spell_earthquake);
DECLARE_SPELL_FUN(spell_enchant_weapon);
DECLARE_SPELL_FUN(spell_energy_drain);
DECLARE_SPELL_FUN(spell_faerie_fire);
DECLARE_SPELL_FUN(spell_faerie_fog);
DECLARE_SPELL_FUN(spell_fireball);
DECLARE_SPELL_FUN(spell_flamestrike);
DECLARE_SPELL_FUN(spell_fly);
DECLARE_SPELL_FUN(spell_gate);
DECLARE_SPELL_FUN(spell_general_purpose);
DECLARE_SPELL_FUN(spell_giant_strength);
DECLARE_SPELL_FUN(spell_harm);
DECLARE_SPELL_FUN(spell_heal);
DECLARE_SPELL_FUN(spell_high_explosive);
DECLARE_SPELL_FUN(spell_identify);
DECLARE_SPELL_FUN(spell_infravision);
DECLARE_SPELL_FUN(spell_invis);
DECLARE_SPELL_FUN(spell_know_alignment);
DECLARE_SPELL_FUN(spell_lightning_bolt);
DECLARE_SPELL_FUN(spell_locate_object);
DECLARE_SPELL_FUN(spell_magic_missile);
DECLARE_SPELL_FUN(spell_mass_invis);
DECLARE_SPELL_FUN(spell_pass_door);
DECLARE_SPELL_FUN(spell_poison);
DECLARE_SPELL_FUN(spell_protection);
DECLARE_SPELL_FUN(spell_refresh);
DECLARE_SPELL_FUN(spell_remove_curse);
DECLARE_SPELL_FUN(spell_sanctuary);
DECLARE_SPELL_FUN(spell_shocking_grasp);
DECLARE_SPELL_FUN(spell_shield);
DECLARE_SPELL_FUN(spell_sleep);
DECLARE_SPELL_FUN(spell_stone_skin);
DECLARE_SPELL_FUN(spell_summon);
DECLARE_SPELL_FUN(spell_teleport);
DECLARE_SPELL_FUN(spell_ventriloquate);
DECLARE_SPELL_FUN(spell_weaken);
DECLARE_SPELL_FUN(spell_word_of_recall);
DECLARE_SPELL_FUN(spell_acid_breath);
DECLARE_SPELL_FUN(spell_fire_breath);
DECLARE_SPELL_FUN(spell_frost_breath);
DECLARE_SPELL_FUN(spell_gas_breath);
DECLARE_SPELL_FUN(spell_lightning_breath);

/* Data files used by the server.

AREA_LIST contains a list of areas to boot.
All files are read in completely at bootup.
Most output files (bug, idea, typo, shutdown) are append-only.

The NULL_FILE is held open so that we have a stream handle in reserve,
so players can go ahead and telnet to all the other descriptors.
Then we close it whenever we need to open a file (e.g. a save file). */

#define PLAYER_DIR      "../player/"    // Player files
//#define NULL_FILE       "/dev/null"   // To reserve one stream
#define MOB_DIR         "MOBProgs/"     // MOBProg files
#define AREA_LIST       "area.lst"      // List of areas

#define BUG_FILE        "bugs.txt"      // For 'bug' and bug()
#define IDEA_FILE       "ideas.txt"     // For 'idea'
#define TYPO_FILE       "typos.txt"     // For 'typo'
#define NOTE_FILE       "notes.txt"     // For 'notes'
#define SHUTDOWN_FILE   "shutdown.txt"  // For 'shutdown'

/* Our function prototypes.
One big lump ... this is every function in Merc. */

// act_comm.c

void add_follower(CHAR_DATA*, CHAR_DATA*);
void stop_follower(CHAR_DATA*);
void die_follower(CHAR_DATA*);
bool is_same_group(CHAR_DATA*, CHAR_DATA*);
bool is_note_to(CHAR_DATA*, NOTE_DATA*);

// act_info.c

void set_title(CHAR_DATA*, char*);

// act_move.c

void move_char(CHAR_DATA*, int);

// act_obj.c

// act_wiz.c

ROOM_INDEX_DATA *find_location(CHAR_DATA*, char*);

// comm.c

void close_socket(DESCRIPTOR_DATA*);
void write_to_buffer(DESCRIPTOR_DATA*, const char*, int);
void send_to_char(const char*, CHAR_DATA*);
void show_string(DESCRIPTOR_DATA*, char*);
void act(const char*, CHAR_DATA*, const void*, const void*, int);

// db.c

void boot_db(void);
void area_update(void);
CHAR_DATA *create_mobile(MOB_INDEX_DATA*);
OBJ_DATA *create_object(OBJ_INDEX_DATA*, int);
void clear_char(CHAR_DATA*);
void free_char(CHAR_DATA*);
char *get_extra_descr(const char*, EXTRA_DESCR_DATA*);
MOB_INDEX_DATA *get_mob_index(int);
OBJ_INDEX_DATA *get_obj_index(int);
ROOM_INDEX_DATA *get_room_index(int);
char fread_letter(FILE*);
int fread_number(FILE*);
char *fread_string(FILE*);
void fread_to_eol(FILE*);
char *fread_word(FILE*);
void *alloc_mem(int);
void *alloc_perm(int);
void free_mem(void*, int);
char *str_dup(const char*);
void free_string(char*);
int number_fuzzy(int);
int number_range(int, int);
int number_percent(void);
int number_door(void);
int number_bits(int);
int number_mm(void);
int dice(int, int);
int interpolate(int, int, int);
void smash_tilde(char*);
bool str_cmp(const char*, const char*);
bool str_prefix(const char*, const char*);
bool str_infix(const char*, const char*);
bool str_suffix(const char*, const char*);
char *capitalize(const char*);
void append_file(CHAR_DATA*, char*, char*);
void bug(const char*, int);
void log_string(const char*);
void tail_chain(void);

// fight.c

void violence_update(void);
void multi_hit(CHAR_DATA*, CHAR_DATA*, int);
void damage(CHAR_DATA*, CHAR_DATA*, int, int);
void update_pos(CHAR_DATA*);
void stop_fighting(CHAR_DATA*, bool);
void death_cry(CHAR_DATA*);
void raw_kill(CHAR_DATA*);

// handler.c

int get_trust(CHAR_DATA*);
int get_age(CHAR_DATA*);
int get_curr_str(CHAR_DATA*);
int get_curr_int(CHAR_DATA*);
int get_curr_wis(CHAR_DATA*);
int get_curr_dex(CHAR_DATA*);
int get_curr_con(CHAR_DATA*);
int can_carry_n(CHAR_DATA*);
int can_carry_w(CHAR_DATA*);
bool is_name(const char*, char*);
void affect_to_char(CHAR_DATA*, AFFECT_DATA*);
void affect_remove(CHAR_DATA*, AFFECT_DATA*);
void affect_strip(CHAR_DATA*, int);
bool is_affected(CHAR_DATA*, int);
void affect_join(CHAR_DATA*, AFFECT_DATA*);
void char_from_room(CHAR_DATA*);
void char_to_room(CHAR_DATA*, ROOM_INDEX_DATA*);
void obj_to_char(OBJ_DATA*, CHAR_DATA*);
void obj_from_char(OBJ_DATA*);
int apply_ac(OBJ_DATA*, int);
OBJ_DATA *get_eq_char(CHAR_DATA*, int);
void equip_char(CHAR_DATA*, OBJ_DATA*, int);
void unequip_char(CHAR_DATA*, OBJ_DATA*);
int count_obj_list(OBJ_INDEX_DATA*, OBJ_DATA*);
void obj_from_room(OBJ_DATA*);
void obj_to_room(OBJ_DATA*, ROOM_INDEX_DATA*);
void obj_to_obj(OBJ_DATA*, OBJ_DATA*);
void obj_from_obj(OBJ_DATA*);
void extract_obj(OBJ_DATA*);
void extract_char(CHAR_DATA*, bool);
CHAR_DATA *get_char_room(CHAR_DATA*, char*);
CHAR_DATA *get_char_world(CHAR_DATA*, char*);
OBJ_DATA *get_obj_type(OBJ_INDEX_DATA*);
OBJ_DATA *get_obj_list(CHAR_DATA*, char*, OBJ_DATA*);
OBJ_DATA *get_obj_carry(CHAR_DATA*, char*);
OBJ_DATA *get_obj_wear(CHAR_DATA*, char*);
OBJ_DATA *get_obj_here(CHAR_DATA*, char*);
OBJ_DATA *get_obj_world(CHAR_DATA*, char*);
OBJ_DATA *create_money(int);
int get_obj_number(OBJ_DATA*);
int get_obj_weight(OBJ_DATA*);
bool room_is_dark(ROOM_INDEX_DATA*);
bool room_is_private(ROOM_INDEX_DATA*);
bool can_see(CHAR_DATA*, CHAR_DATA*);
bool can_see_obj(CHAR_DATA*, OBJ_DATA*);
bool can_drop_obj(CHAR_DATA*, OBJ_DATA*);
char *item_type_name(OBJ_DATA*);
char *affect_loc_name(int);
char *affect_bit_name(int);
char *extra_bit_name(int);

// interp.c

void interpret(CHAR_DATA*, char*);
bool is_number(char*);
int number_argument(char*, char*);
char *one_argument(char*, char*);
bool IS_SWITCHED(CHAR_DATA*);

// magic.c

int skill_lookup(const char*);
int slot_lookup(int);
bool saves_spell(int, CHAR_DATA*);
void obj_cast_spell(int, int, CHAR_DATA*, CHAR_DATA*, OBJ_DATA*);

// mob_prog.c

void mprog_wordlist_check(char *, CHAR_DATA*, CHAR_DATA*, OBJ_DATA*, void*, int);
void mprog_percent_check(CHAR_DATA*, CHAR_DATA*, OBJ_DATA*, void*, int);
void mprog_act_trigger(char*, CHAR_DATA*, CHAR_DATA*, OBJ_DATA*, void*);
void mprog_bribe_trigger(CHAR_DATA*, CHAR_DATA*, int);
void mprog_entry_trigger(CHAR_DATA*);
void mprog_give_trigger(CHAR_DATA*, CHAR_DATA*, OBJ_DATA*);
void mprog_greet_trigger(CHAR_DATA*);
void mprog_fight_trigger(CHAR_DATA*, CHAR_DATA*);
void mprog_hitprcnt_trigger(CHAR_DATA*, CHAR_DATA*);
void mprog_death_trigger(CHAR_DATA*);
void mprog_random_trigger(CHAR_DATA*);
void mprog_speech_trigger(char*, CHAR_DATA*);

// save.c

void save_char_obj(CHAR_DATA*);
bool load_char_obj(DESCRIPTOR_DATA*, char*);

// special.c

SPEC_FUN *spec_lookup(const char *);

// update.c

void advance_level(CHAR_DATA*);
void gain_exp(CHAR_DATA*, int gain);
void gain_condition(CHAR_DATA*, int iCond, int value);
void update_handler(void);
