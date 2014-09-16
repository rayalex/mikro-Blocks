#include "Tetris_objects.h"
#include "Tetris_resources.h"
#include "built_in.h"



// Touch Panel module connections
sbit TSC2006_CS at GPIO_PORTJ_AHB_DATA.B7;
sbit TSC2006_CS_Direction at GPIO_PORTJ_AHB_DIR.B7;
sbit TSC2006_RST at GPIO_PORTH_AHB_DATA.B5;
sbit TSC2006_RST_Direction at GPIO_PORTH_AHB_DIR.B5;
// End Touch Panel module connections

// Global variables
unsigned int Xcoord, Ycoord;
char PenDown;
typedef unsigned long TPointer;
TPointer PressedObject;
int PressedObjectType;
unsigned int display_width, display_height;

int _object_count;
unsigned short object_pressed;
TCImage *local_cimage;
TCImage *exec_cimage;
int cimage_order;
TBox *local_box;
TBox *exec_box;
int box_order;

void TFTIntern_Config()  {
unsigned short rotation;
  TLCDRasterTiming Timings;
  GPIO_Alternate_Function_Enable(&_GPIO_MODULE_LCD_RASTER_24BPP);
  SYSCTL_RCGCLCD = 0x01;
  TFTIntern_ModeSet(_TFT_INTERN_MODE_RASTER | _TFT_INTERN_MODE_AUTO_UFLOW_RESTART, 40000000, 120000000);
  TFTIntern_RasterConfigSet(_RASTER_FMT_ACTIVE_PALETTIZED_16BIT | _RASTER_LOAD_DATA_ONLY, 0);
  Timings.config =  _RASTER_TIMING_SYNCS_ON_RISING_PIXCLK | _RASTER_TIMING_ACTIVE_HIGH_OE | _RASTER_TIMING_ACTIVE_HIGH_PIXCLK | _RASTER_TIMING_ACTIVE_HIGH_HSYNC | _RASTER_TIMING_ACTIVE_HIGH_VSYNC;
  Timings.displayWidth  = 800;
  Timings.displayHeight = 480;
  Timings.horizontalFrontPorch = 40;
  Timings.horizontalBackPorch  = 40;
  Timings.horizontalSyncWidth  = 48;
  Timings.verticalFrontPorch   = 13;
  Timings.verticalBackPorch    = 29;
  Timings.verticalSyncWidth    = 3;
  Timings.biasLineCount        = 0;
  TFTIntern_RasterTimingSet(&Timings);
  rotation = TFTIntern_GetDisplayRotation();
  if (rotation == _TFT_INTERN_ROTATE_270)
    TFTIntern_SetDisplayRotation(_TFT_INTERN_ROTATE_0);
  else
    TFTIntern_SetDisplayRotation(rotation + 1);
  TFTIntern_SetDisplaySize(800, 480);
  TFTIntern_DMAConfigSet(_TFT_INTERN_DMA_PRIORITY_7 | _TFT_INTERN_DMA_FIFORDY_512_WORDS | _TFT_INTERN_DMA_BURST_16 | _TFT_INTERN_DMA_BYTE_ORDER_0123 | _TFT_INTERN_DMA_PING_PONG);
  TFTIntern_RasterFrameBufferSet(0, (unsigned long *)(0x10000000+0x0),768000);
  TFTIntern_RasterFrameBufferSet(1, (unsigned long *)(0x10000000+0x0),768000);
  TFTIntern_RasterPaletteSet(_TFT_INTERN_PALETTE_TYPE_DIRECT, (unsigned long *)(0x10000000), 0, 0, 0);
  TFTIntern_SetMemoryStartAddr((unsigned int *)(0x10000000));
  TFTIntern_RasterEnable();
}


void TSC2006_Config()  {
unsigned short rotation;
  GPIO_Digital_Output(&GPIO_PORTJ_AHB_DATA, _GPIO_PINMASK_7);
  GPIO_Digital_Output(&GPIO_PORTH_AHB_DATA, _GPIO_PINMASK_5);
  TSC2006_Reset();
  TSC2006_WriteCB1(_TSC2006_CB1_RES_12_BIT | _TSC2006_CB1_STOP |  _TSC2006_CB1_SWRST);
  TSC2006_WriteCB1(0);
  TSC2006_WriteCB0(_TSC2006_CB0_CMD_WRITE_REG |  _TSC2006_CB0_ADDR_CFR0,
                    (_TSC2006_CFR0_TSC_CONV |  _TSC2006_CFR0_STS_NRML | _TSC2006_CFR0_RM_12_BIT | _TSC2006_CFR0_CLK_4MHz | _TSC2006_CFR0_PVS_500uS | 
                    _TSC2006_CFR0_PRE_340us | _TSC2006_CFR0_SNS_96us | _TSC2006_CFR0_LSM_ON));
  TSC2006_WriteCB0(_TSC2006_CB0_CMD_WRITE_REG, _TSC2006_CB0_ADDR_CFR1  | _TSC2006_CFR1_BTD_0mS);
  TSC2006_WriteCB0(_TSC2006_CB0_CMD_WRITE_REG | _TSC2006_CB0_ADDR_CFR2, (_TSC2006_CFR2_DAV1 | _TSC2006_CFR2_ZONE_DIS | _TSC2006_CFR2_MAVE_DIS));
  TSC2006_WriteCB1(_TSC2006_CB1_RES_12_BIT | _TSC2006_CB1_CF_XYZ1Z2_SCAN);
  TSC2006_SetTouchPressureThreshold(8000);
  rotation = TSC2006_GetTPRotation();
  if (rotation == _TSC2006_ROTATE_270)
    TSC2006_SetTPRotation(_TSC2006_ROTATE_0);
  else
    TSC2006_SetTPRotation(rotation + 1);
  TSC2006_SetDisplaySize(480, 800);
}

static void InitializeTouchPanel() {
  TFTIntern_Config();
  TSC2006_Config();


  PenDown = 0;
  PressedObject = 0;
  PressedObjectType = -1;
}

//--------------------- User code ---------------------//
void CalibrateIntern() {
  TSC2006_SetCalibrationConsts(186, 3874, 242, 3842);
}

//----------------- End of User code ------------------//

void Calibrate() {
  TFTIntern_Fill_Screen(CL_BLACK_Intern);
  TFTIntern_Set_Font(TFTIntern_defaultFont, CL_WHITE_Intern, FO_HORIZONTAL_Intern);
  TFTIntern_Write_Text("Touch points on the screen as they", 125, 385);
  TFTIntern_Write_Text("appear to calibrate touchscreen.", 135, 415);
  TFTIntern_Set_Brush(1, CL_WHITE_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(0, 0, 3);
  TSC2006_CalibratePoint(_TSC2006_DISP_0_0);
  Delay_ms(500);
  TFTIntern_Set_Brush(1, CL_BLACK_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(0, 0, 3);
  TFTIntern_Set_Brush(1, CL_WHITE_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(TFTIntern_DisplayHeight-1, 0, 3);
  TSC2006_CalibratePoint(_TSC2006_DISP_XMAX_0);
  Delay_ms(500);
  TFTIntern_Set_Brush(1, CL_BLACK_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(TFTIntern_DisplayHeight-1, 0, 3);
  TFTIntern_Set_Brush(1, CL_WHITE_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(TFTIntern_DisplayHeight-1, TFTIntern_DisplayWidth-1, 3);
  TSC2006_CalibratePoint(_TSC2006_DISP_XMAX_YMAX);
  Delay_ms(500);
  TFTIntern_Set_Brush(1, CL_BLACK_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(TFTIntern_DisplayHeight-1, TFTIntern_DisplayWidth-1, 3);
  TFTIntern_Set_Brush(1, CL_WHITE_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(0, TFTIntern_DisplayWidth-1, 3);
  TSC2006_CalibratePoint(_TSC2006_DISP_0_YMAX);
  TFTIntern_Set_Brush(1, CL_BLACK_Intern, 0, 0, 0, 0);
  TFTIntern_Circle(0, TFTIntern_DisplayWidth-1, 3);
  Delay_ms(500);
}


/////////////////////////
  TScreen*  CurrentScreen;

  TScreen                GameScreen;
  TBox                   FauxBox;
  TCImage               ImgArrowDown = 
         {
         &GameScreen           , //   ImgArrowDown.OwnerScreen
         1                     , //   ImgArrowDown.Order          
         80                    , //   ImgArrowDown.Left           
         720                   , //   ImgArrowDown.Top            
         320                   , //   ImgArrowDown.Width          
         80                    , //   ImgArrowDown.Height         
         &button_down_bmp      , //   ImgArrowDown.Picture_Name  
         1                     , //   ImgArrowDown.Visible      
         1                     , //   ImgArrowDown.Active         
         0                     , //   ImgArrowDown.Picture_Type        
         1                     , //   ImgArrowDown.Picture_Ratio       
         ImgArrowDownOnUp      , //   ImgArrowDown.OnUpPtr
         ImgArrowDownOnDown    , //   ImgArrowDown.OnDownPtr
         0                     , //   ImgArrowDown.OnClickPtr
         0                       //   ImgArrowDown.OnPressPtr
         };
  TCImage               ImgArrowLeft = 
         {
         &GameScreen           , //   ImgArrowLeft.OwnerScreen
         2                     , //   ImgArrowLeft.Order          
         0                     , //   ImgArrowLeft.Left           
         555                   , //   ImgArrowLeft.Top            
         80                    , //   ImgArrowLeft.Width          
         80                    , //   ImgArrowLeft.Height         
         &button_left_bmp      , //   ImgArrowLeft.Picture_Name  
         1                     , //   ImgArrowLeft.Visible      
         1                     , //   ImgArrowLeft.Active         
         0                     , //   ImgArrowLeft.Picture_Type        
         1                     , //   ImgArrowLeft.Picture_Ratio       
         ImgArrowLeftOnUp      , //   ImgArrowLeft.OnUpPtr
         ImgArrowLeftOnDown    , //   ImgArrowLeft.OnDownPtr
         0                     , //   ImgArrowLeft.OnClickPtr
         0                       //   ImgArrowLeft.OnPressPtr
         };
  TCImage               ImgArrowRight = 
         {
         &GameScreen           , //   ImgArrowRight.OwnerScreen
         3                     , //   ImgArrowRight.Order          
         400                   , //   ImgArrowRight.Left           
         555                   , //   ImgArrowRight.Top            
         80                    , //   ImgArrowRight.Width          
         80                    , //   ImgArrowRight.Height         
         &button_right_bmp     , //   ImgArrowRight.Picture_Name  
         1                     , //   ImgArrowRight.Visible      
         1                     , //   ImgArrowRight.Active         
         0                     , //   ImgArrowRight.Picture_Type        
         1                     , //   ImgArrowRight.Picture_Ratio       
         ImgArrowRightOnUp     , //   ImgArrowRight.OnUpPtr
         ImgArrowRightOnDown   , //   ImgArrowRight.OnDownPtr
         0                     , //   ImgArrowRight.OnClickPtr
         0                       //   ImgArrowRight.OnPressPtr
         };
  TCImage               ImgRotateCCW = 
         {
         &GameScreen           , //   ImgRotateCCW.OwnerScreen
         4                     , //   ImgRotateCCW.Order          
         0                     , //   ImgRotateCCW.Left           
         405                   , //   ImgRotateCCW.Top            
         80                    , //   ImgRotateCCW.Width          
         80                    , //   ImgRotateCCW.Height         
         &button_ccw_bmp       , //   ImgRotateCCW.Picture_Name  
         1                     , //   ImgRotateCCW.Visible      
         1                     , //   ImgRotateCCW.Active         
         0                     , //   ImgRotateCCW.Picture_Type        
         1                     , //   ImgRotateCCW.Picture_Ratio       
         ImgRotateCCWOnUp      , //   ImgRotateCCW.OnUpPtr
         ImgRotateCCWOnDown    , //   ImgRotateCCW.OnDownPtr
         0                     , //   ImgRotateCCW.OnClickPtr
         0                       //   ImgRotateCCW.OnPressPtr
         };
  TCImage               ImgRotateCW = 
         {
         &GameScreen           , //   ImgRotateCW.OwnerScreen
         5                     , //   ImgRotateCW.Order          
         400                   , //   ImgRotateCW.Left           
         405                   , //   ImgRotateCW.Top            
         80                    , //   ImgRotateCW.Width          
         80                    , //   ImgRotateCW.Height         
         &button_cw_bmp        , //   ImgRotateCW.Picture_Name  
         1                     , //   ImgRotateCW.Visible      
         1                     , //   ImgRotateCW.Active         
         0                     , //   ImgRotateCW.Picture_Type        
         1                     , //   ImgRotateCW.Picture_Ratio       
         ImgRotateCWOnUp       , //   ImgRotateCW.OnUpPtr
         ImgRotateCWOnDown     , //   ImgRotateCW.OnDownPtr
         0                     , //   ImgRotateCW.OnClickPtr
         0                       //   ImgRotateCW.OnPressPtr
         };
  TCImage               ImgScore = 
         {
         &GameScreen           , //   ImgScore.OwnerScreen
         6                     , //   ImgScore.Order          
         80                    , //   ImgScore.Left           
         27                    , //   ImgScore.Top            
         320                   , //   ImgScore.Width          
         17                    , //   ImgScore.Height         
         &lblScoreStart_bmp    , //   ImgScore.Picture_Name  
         1                     , //   ImgScore.Visible      
         1                     , //   ImgScore.Active         
         0                     , //   ImgScore.Picture_Type        
         1                     , //   ImgScore.Picture_Ratio       
         0                     , //   ImgScore.OnUpPtr
         0                     , //   ImgScore.OnDownPtr
         0                     , //   ImgScore.OnClickPtr
         0                       //   ImgScore.OnPressPtr
         };
  TCImage                * const code Screen1_CImages[6]=
         {
         &ImgArrowDown,        
         &ImgArrowLeft,        
         &ImgArrowRight,       
         &ImgRotateCCW,        
         &ImgRotateCW,         
         &ImgScore             
         };
  TBox                   * const code Screen1_Boxes[1]=
         {
         &FauxBox              
         };

  TScreen                HomeScreen;
  TCImage               ImgLogo = 
         {
         &HomeScreen           , //   ImgLogo.OwnerScreen
         0                     , //   ImgLogo.Order          
         20                    , //   ImgLogo.Left           
         234                   , //   ImgLogo.Top            
         440                   , //   ImgLogo.Width          
         135                   , //   ImgLogo.Height         
         &logo_bmp             , //   ImgLogo.Picture_Name  
         1                     , //   ImgLogo.Visible      
         1                     , //   ImgLogo.Active         
         0                     , //   ImgLogo.Picture_Type        
         1                     , //   ImgLogo.Picture_Ratio       
         0                     , //   ImgLogo.OnUpPtr
         0                     , //   ImgLogo.OnDownPtr
         0                     , //   ImgLogo.OnClickPtr
         0                       //   ImgLogo.OnPressPtr
         };
  TCImage               ImgStart = 
         {
         &HomeScreen           , //   ImgStart.OwnerScreen
         1                     , //   ImgStart.Order          
         115                   , //   ImgStart.Left           
         575                   , //   ImgStart.Top            
         250                   , //   ImgStart.Width          
         80                    , //   ImgStart.Height         
         &btn_start_bmp        , //   ImgStart.Picture_Name  
         1                     , //   ImgStart.Visible      
         1                     , //   ImgStart.Active         
         0                     , //   ImgStart.Picture_Type        
         1                     , //   ImgStart.Picture_Ratio       
         0                     , //   ImgStart.OnUpPtr
         0                     , //   ImgStart.OnDownPtr
         ImgStartOnClick       , //   ImgStart.OnClickPtr
         0                       //   ImgStart.OnPressPtr
         };
  TCImage                * const code Screen2_CImages[2]=
         {
         &ImgLogo,             
         &ImgStart             
         };

  TScreen                EndScreen;
  TCImage               ImgHome = 
         {
         &EndScreen            , //   ImgHome.OwnerScreen
         0                     , //   ImgHome.Order          
         210                   , //   ImgHome.Left           
         700                   , //   ImgHome.Top            
         64                    , //   ImgHome.Width          
         64                    , //   ImgHome.Height         
         &end_back_button_bmp  , //   ImgHome.Picture_Name  
         1                     , //   ImgHome.Visible      
         1                     , //   ImgHome.Active         
         0                     , //   ImgHome.Picture_Type        
         1                     , //   ImgHome.Picture_Ratio       
         0                     , //   ImgHome.OnUpPtr
         0                     , //   ImgHome.OnDownPtr
         ImgHomeOnClick        , //   ImgHome.OnClickPtr
         0                       //   ImgHome.OnPressPtr
         };
  TCImage               ImgEndFace = 
         {
         &EndScreen            , //   ImgEndFace.OwnerScreen
         1                     , //   ImgEndFace.Order          
         177                   , //   ImgEndFace.Left           
         173                   , //   ImgEndFace.Top            
         126                   , //   ImgEndFace.Width          
         110                   , //   ImgEndFace.Height         
         &end_face_bmp         , //   ImgEndFace.Picture_Name  
         1                     , //   ImgEndFace.Visible      
         1                     , //   ImgEndFace.Active         
         0                     , //   ImgEndFace.Picture_Type        
         1                     , //   ImgEndFace.Picture_Ratio       
         0                     , //   ImgEndFace.OnUpPtr
         0                     , //   ImgEndFace.OnDownPtr
         0                     , //   ImgEndFace.OnClickPtr
         0                       //   ImgEndFace.OnPressPtr
         };
  TCImage               ImgEndGameOver = 
         {
         &EndScreen            , //   ImgEndGameOver.OwnerScreen
         2                     , //   ImgEndGameOver.Order          
         147                   , //   ImgEndGameOver.Left           
         390                   , //   ImgEndGameOver.Top            
         185                   , //   ImgEndGameOver.Width          
         21                    , //   ImgEndGameOver.Height         
         &end_text_bmp         , //   ImgEndGameOver.Picture_Name  
         1                     , //   ImgEndGameOver.Visible      
         1                     , //   ImgEndGameOver.Active         
         0                     , //   ImgEndGameOver.Picture_Type        
         1                     , //   ImgEndGameOver.Picture_Ratio       
         0                     , //   ImgEndGameOver.OnUpPtr
         0                     , //   ImgEndGameOver.OnDownPtr
         0                     , //   ImgEndGameOver.OnClickPtr
         0                       //   ImgEndGameOver.OnPressPtr
         };
  TCImage                * const code Screen3_CImages[3]=
         {
         &ImgHome,             
         &ImgEndFace,          
         &ImgEndGameOver       
         };



static void InitializeObjects() {
  GameScreen.Color                     = 0x18E4;
  GameScreen.Width                     = 480;
  GameScreen.Height                    = 800;
  GameScreen.CImagesCount              = 6;
  GameScreen.CImages                   = Screen1_CImages;
  GameScreen.BoxesCount                = 1;
  GameScreen.Boxes                     = Screen1_Boxes;
  GameScreen.ObjectsCount              = 7;

  HomeScreen.Color                     = 0x18E4;
  HomeScreen.Width                     = 480;
  HomeScreen.Height                    = 800;
  HomeScreen.CImagesCount              = 2;
  HomeScreen.CImages                   = Screen2_CImages;
  HomeScreen.BoxesCount                = 0;
  HomeScreen.ObjectsCount              = 2;

  EndScreen.Color                     = 0x18E4;
  EndScreen.Width                     = 480;
  EndScreen.Height                    = 800;
  EndScreen.CImagesCount              = 3;
  EndScreen.CImages                   = Screen3_CImages;
  EndScreen.BoxesCount                = 0;
  EndScreen.ObjectsCount              = 3;


  FauxBox.OwnerScreen     = &GameScreen;
  FauxBox.Order           = 0;
  FauxBox.Left            = 0;
  FauxBox.Top             = 0;
  FauxBox.Width           = 480;
  FauxBox.Height          = 800;
  FauxBox.Pen_Width       = 1;
  FauxBox.Pen_Color       = 0x18E4;
  FauxBox.Visible         = 1;
  FauxBox.Active          = 1;
  FauxBox.Transparent     = 1;
  FauxBox.Gradient        = 0;
  FauxBox.Gradient_Orientation = 0;
  FauxBox.Gradient_Start_Color = 0xFFFF;
  FauxBox.Gradient_End_Color = 0xC618;
  FauxBox.Color           = 0x18E4;
  FauxBox.PressColEnabled = 0;
  FauxBox.Press_Color     = 0xE71C;
  FauxBox.OnUpPtr         = FauxBoxOnUp;
  FauxBox.OnDownPtr       = 0;
  FauxBox.OnClickPtr      = 0;
  FauxBox.OnPressPtr      = 0;
}

static char IsInsideObject (unsigned int X, unsigned int Y, unsigned int Left, unsigned int Top, unsigned int Width, unsigned int Height) { // static
  if ( (Left<= X) && (Left+ Width - 1 >= X) &&
       (Top <= Y)  && (Top + Height - 1 >= Y) )
    return 1;
  else
    return 0;
}


#define GetCImage(index)              CurrentScreen->CImages[index]
#define GetBox(index)                 CurrentScreen->Boxes[index]


void DrawCImage(TCImage *AImage) {
  if (AImage->Visible) {
    TFTIntern_Image(AImage->Left, AImage->Top, AImage->Picture_Name, AImage->Picture_Ratio);
  }
}

void DrawBox(TBox *ABox) {
  if (ABox->Visible == 1) {
    if (object_pressed == 1) {
      object_pressed = 0;
      TFTIntern_Set_Brush(ABox->Transparent, ABox->Press_Color, ABox->Gradient, ABox->Gradient_Orientation, ABox->Gradient_End_Color, ABox->Gradient_Start_Color);
    }
    else {
      TFTIntern_Set_Brush(ABox->Transparent, ABox->Color, ABox->Gradient, ABox->Gradient_Orientation, ABox->Gradient_Start_Color, ABox->Gradient_End_Color);
    }
    TFTIntern_Set_Pen(ABox->Pen_Color, ABox->Pen_Width);
    TFTIntern_Rectangle(ABox->Left, ABox->Top, ABox->Left + ABox->Width - 1, ABox->Top + ABox->Height - 1);
  }
}

void DrawScreen(TScreen *aScreen) {
 unsigned int order;
  unsigned short rotation;
  unsigned short cimage_idx;
  TCImage *local_cimage;
  unsigned short box_idx;
  TBox *local_box;

  object_pressed = 0;
  order = 0;
  cimage_idx = 0;
  box_idx = 0;
  CurrentScreen = aScreen;

  if ((display_width != CurrentScreen->Width) || (display_height != CurrentScreen->Height)) {
    rotation = TFTIntern_GetDisplayRotation();
    if (display_width > CurrentScreen->Width) {
      if (rotation == _TFT_INTERN_ROTATE_270)
        rotation = _TFT_INTERN_ROTATE_0;
      else
        rotation++;
    } else {
      if (rotation == _TFT_INTERN_ROTATE_0)
        rotation = _TFT_INTERN_ROTATE_270;
      else
        rotation--;
    }
    TFTIntern_SetDisplayRotation(rotation);
    TSC2006_SetDisplaySize(CurrentScreen->Width, CurrentScreen->Height);
    TFTIntern_Fill_Screen(CurrentScreen->Color);
    display_width = CurrentScreen->Width;
    display_height = CurrentScreen->Height;
  }
  else
    TFTIntern_Fill_Screen(CurrentScreen->Color);


  while (order < CurrentScreen->ObjectsCount) {
    if (box_idx < CurrentScreen->BoxesCount) {
      local_box = GetBox(box_idx);
      if (order == local_box->Order) {
        box_idx++;
        order++;
        DrawBox(local_box);
      }
    }

    if (cimage_idx < CurrentScreen->CImagesCount) {
      local_cimage = GetCImage(cimage_idx);
      if (order == local_cimage->Order) {
        cimage_idx++;
        order++;
        DrawCImage(local_cimage);
      }
    }

  }
}

void Get_Object(unsigned int X, unsigned int Y) {
  cimage_order        = -1;
  box_order           = -1;
  //  CImages
  for ( _object_count = 0 ; _object_count < CurrentScreen->CImagesCount ; _object_count++ ) {
    local_cimage = GetCImage(_object_count);
    if (local_cimage->Active == 1) {
      if (IsInsideObject(X, Y, local_cimage->Left, local_cimage->Top,
                         local_cimage->Width, local_cimage->Height) == 1) {
        cimage_order = local_cimage->Order;
        exec_cimage = local_cimage;
      }
    }
  }

  //  Boxes
  for ( _object_count = 0 ; _object_count < CurrentScreen->BoxesCount ; _object_count++ ) {
    local_box = GetBox(_object_count);
    if (local_box->Active == 1) {
      if (IsInsideObject(X, Y, local_box->Left, local_box->Top,
                         local_box->Width, local_box->Height) == 1) {
        box_order = local_box->Order;
        exec_box = local_box;
      }
    }
  }

  _object_count = -1;
  if (cimage_order >  _object_count )
    _object_count = cimage_order;
  if (box_order >  _object_count )
    _object_count = box_order;
}


void Process_TP_Press(unsigned int X, unsigned int Y) {
  exec_cimage         = 0;
  exec_box            = 0;

  Get_Object(X, Y);

  if (_object_count != -1) {
    if (_object_count == cimage_order) {
      if (exec_cimage->Active == 1) {
        if (exec_cimage->OnPressPtr != 0) {
          exec_cimage->OnPressPtr();
          return;
        }
      }
    }

    if (_object_count == box_order) {
      if (exec_box->Active == 1) {
        if (exec_box->OnPressPtr != 0) {
          exec_box->OnPressPtr();
          return;
        }
      }
    }

  }
}

void Process_TP_Up(unsigned int X, unsigned int Y) {

  switch (PressedObjectType) {
    // Box
    case 6: {
      if (PressedObject != 0) {
        exec_box = (TBox*)PressedObject;
        if ((exec_box->PressColEnabled == 1) && (exec_box->OwnerScreen == CurrentScreen)) {
          DrawBox(exec_box);
        }
        break;
      }
      break;
    }
  }

  exec_cimage          = 0;

  Get_Object(X, Y);


  if (_object_count != -1) {
  // CImages
    if (_object_count == cimage_order) {
      if (exec_cimage->Active == 1) {
        if (exec_cimage->OnUpPtr != 0)
          exec_cimage->OnUpPtr();
        if (PressedObject == (TPointer)exec_cimage)
          if (exec_cimage->OnClickPtr != 0)
            exec_cimage->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  // Boxes
    if (_object_count == box_order) {
      if (exec_box->Active == 1) {
        if (exec_box->OnUpPtr != 0)
          exec_box->OnUpPtr();
        if (PressedObject == (TPointer)exec_box)
          if (exec_box->OnClickPtr != 0)
            exec_box->OnClickPtr();
        PressedObject = 0;
        PressedObjectType = -1;
        return;
      }
    }

  }
  PressedObject = 0;
  PressedObjectType = -1;
}

void Process_TP_Down(unsigned int X, unsigned int Y) {

  object_pressed      = 0;
  exec_cimage         = 0;
  exec_box            = 0;

  Get_Object(X, Y);

  if (_object_count != -1) {
    if (_object_count == cimage_order) {
      if (exec_cimage->Active == 1) {
        PressedObject = (TPointer)exec_cimage;
        PressedObjectType = 11;
        if (exec_cimage->OnDownPtr != 0) {
          exec_cimage->OnDownPtr();
          return;
        }
      }
    }

    if (_object_count == box_order) {
      if (exec_box->Active == 1) {
        if (exec_box->PressColEnabled == 1) {
          object_pressed = 1;
          DrawBox(exec_box);
        }
        PressedObject = (TPointer)exec_box;
        PressedObjectType = 6;
        if (exec_box->OnDownPtr != 0) {
          exec_box->OnDownPtr();
          return;
        }
      }
    }

  }
}

void Check_TP() {
  if (TSC2006_PressDetect()) {
    if (TSC2006_GetCoordinates(&Xcoord, &Ycoord) == 0) {
    // After a PRESS is detected read X-Y and convert it to Display dimensions space
      Process_TP_Press(Xcoord, Ycoord);
      if (PenDown == 0) {
        PenDown = 1;
        Process_TP_Down(Xcoord, Ycoord);
      }
    }
  }
  else if (PenDown == 1) {
    PenDown = 0;
    Process_TP_Up(Xcoord, Ycoord);
  }
}

void Init_MCU() {
  // Place your code here
  GPIO_Digital_Output(&GPIO_PORTM, _GPIO_PINMASK_6);
  GPIO_PORTM_DATA.B6 = 1;
  SPI1_Init_Advanced(15000000, _SPI_MASTER, _SPI_8_BIT | _SPI_CLK_IDLE_LOW | _SPI_FIRST_CLK_EDGE_TRANSITION, &_GPIO_MODULE_SPI1_B5E45_AHB);
  uDMA_Init();
  EPI_SDRAM_Init();
  TFTIntern_SetDefaultMode();
  TSC2006_SetDefaultMode();
  TFTIntern_SetDisplayRotation(_TFT_INTERN_ROTATE_0);
}

void Start_TP() {
  Init_MCU();

  InitializeTouchPanel();

  // Delay_ms(1000);
  // TFTIntern_Fill_Screen(0);
  CalibrateIntern();
  // TFTIntern_Fill_Screen(0);

  InitializeObjects();
  display_width = HomeScreen.Width;
  display_height = HomeScreen.Height;
  DrawScreen(&HomeScreen);
}
