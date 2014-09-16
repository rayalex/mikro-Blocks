typedef struct Screen TScreen;

typedef struct {
  unsigned long  config;
  unsigned int   displayWidth;
  unsigned int   displayHeight;
  unsigned int   horizontalFrontPorch;
  unsigned int   horizontalBackPorch;
  unsigned int   horizontalSyncWidth;
  unsigned short verticalFrontPorch;
  unsigned short verticalBackPorch;
  unsigned short verticalSyncWidth;
  unsigned short biasLineCount;
} TLCDRasterTiming;

typedef struct {
  unsigned int X_Min;
  unsigned int X_Max;
  unsigned int Y_Min;
  unsigned int Y_Max;
  char         Rotate;
} TTPConstants;

typedef const struct  CImage {
  TScreen*  OwnerScreen;
  char          Order;
  unsigned int  Left;
  unsigned int  Top;
  unsigned int  Width;
  unsigned int  Height;
  const char    *Picture_Name;
  char          Visible;
  char          Active;
  char          Picture_Type;
  char          Picture_Ratio;
  void          (*OnUpPtr)();
  void          (*OnDownPtr)();
  void          (*OnClickPtr)();
  void          (*OnPressPtr)();
} TCImage;

typedef struct  Box {
  TScreen*  OwnerScreen;
  char          Order;
  unsigned int  Left;
  unsigned int  Top;
  unsigned int  Width;
  unsigned int  Height;
  char          Pen_Width;
  unsigned int  Pen_Color;
  char          Visible;
  char          Active;
  char          Transparent;
  char          Gradient;
  char          Gradient_Orientation;
  unsigned int  Gradient_Start_Color;
  unsigned int  Gradient_End_Color;
  unsigned int  Color;
  char          PressColEnabled;
  unsigned int  Press_Color;
  void          (*OnUpPtr)();
  void          (*OnDownPtr)();
  void          (*OnClickPtr)();
  void          (*OnPressPtr)();
} TBox;

struct Screen {
  unsigned int           Color;
  unsigned int           Width;
  unsigned int           Height;
  unsigned int           ObjectsCount;
  unsigned int           CImagesCount;
  TCImage                * const code *CImages;
  unsigned int           BoxesCount;
  TBox                   * const code *Boxes;
};

extern   TScreen                GameScreen;
extern   TBox                   FauxBox;
extern   TCImage               ImgArrowDown;
extern   TCImage               ImgArrowLeft;
extern   TCImage               ImgArrowRight;
extern   TCImage               ImgRotateCCW;
extern   TCImage               ImgRotateCW;
extern   TCImage               ImgScore;
extern   TCImage                * const code Screen1_CImages[6];
extern   TBox                   * const code Screen1_Boxes[1];

extern   TScreen                HomeScreen;
extern   TCImage               ImgLogo;
extern   TCImage               ImgStart;
extern   TCImage                * const code Screen2_CImages[2];

extern   TScreen                EndScreen;
extern   TCImage               ImgHome;
extern   TCImage               ImgEndFace;
extern   TCImage               ImgEndGameOver;
extern   TCImage                * const code Screen3_CImages[3];


/////////////////////////
// Events Code Declarations
void FauxBoxOnUp();
void ImgArrowDownOnDown();
void ImgArrowDownOnUp();
void ImgArrowLeftOnDown();
void ImgArrowLeftOnUp();
void ImgArrowRightOnDown();
void ImgArrowRightOnUp();
void ImgHomeOnClick();
void ImgRotateCCWOnDown();
void ImgRotateCCWOnUp();
void ImgRotateCWOnDown();
void ImgRotateCWOnUp();
void ImgStartOnClick();
/////////////////////////

/////////////////////////////////
// Caption variables Declarations
extern char FauxBox_Caption[];
extern const char ImgArrowDown_Caption[];
extern const char ImgArrowLeft_Caption[];
extern const char ImgArrowRight_Caption[];
extern const char ImgRotateCCW_Caption[];
extern const char ImgRotateCW_Caption[];
extern const char ImgScore_Caption[];
extern const char ImgLogo_Caption[];
extern const char ImgStart_Caption[];
extern const char ImgHome_Caption[];
extern const char ImgEndFace_Caption[];
extern const char ImgEndGameOver_Caption[];
/////////////////////////////////

void DrawScreen(TScreen *aScreen);
void DrawCImage(TCImage *ACimage);
void DrawBox(TBox *ABox);
void Check_TP();
void Start_TP();
void Process_TP_Press(unsigned int X, unsigned int Y);
void Process_TP_Up(unsigned int X, unsigned int Y);
void Process_TP_Down(unsigned int X, unsigned int Y);
