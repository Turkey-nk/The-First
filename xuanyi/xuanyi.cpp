/*
 * 古堡谜案 - 悬疑推理探案游戏（EasyX图形版）
 * 环境：Visual Studio + EasyX 图形库
 * 游戏玩法：玩家扮演侦探，调查古堡谋杀案，收集线索后指认真凶
 * 特色：带有图像资源的可视化界面，支持鼠标交互
 */

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <graphics.h>
#include <stdio.h>
#include <tchar.h>



// 窗口尺寸常量
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// 游戏状态枚举：定义游戏的各个场景
enum GameState {
    STATE_TITLE,        // 标题界面
    STATE_INTRO,        // 游戏介绍界面
    STATE_GAME,         // 主游戏调查界面
    STATE_QUESTION,     // 询问嫌疑人界面
    STATE_ACCUSE,       // 指认凶手界面
    STATE_ENDING_WIN,   // 胜利结局
    STATE_ENDING_LOSE   // 失败结局
};

// 嫌疑人枚举：定义各个嫌疑人
enum Suspect {
    NONE,               // 无选择
    SUSPECT_WIFE,       // 男爵夫人
    SUSPECT_JACK,       // 养子杰克
    SUSPECT_BUTLER      // 老管家
};

// 图像资源变量声明
IMAGE imgBackground;    // 背景图
IMAGE imgWife;          // 男爵夫人图像
IMAGE imgJack;          // 养子杰克图像
IMAGE imgButler;        // 老管家图像
IMAGE imgCandle;        // 烛台图像（凶器）
IMAGE imgLetter;        // 信件遗嘱图像
IMAGE imgScarf;         // 女士围巾图像
IMAGE imgFootprint;     // 泥脚印图像
IMAGE imgBody;          // 男爵尸体图像
IMAGE imgDialogBox;     // 对话框背景图
IMAGE imgClueCard;      // 线索卡片背景图
IMAGE imgTitle;         // 标题图像
IMAGE imgAccuseBtn;     // 指认按钮图像
IMAGE imgWinEnd;        // 胜利结局图像
IMAGE imgLoseEnd;       // 失败结局图像

// 游戏状态变量
GameState currentState = STATE_TITLE;  // 当前游戏状态
Suspect currentSuspect = NONE;         // 当前选中的嫌疑人

// 线索收集状态
bool clueCandle = false;       // 烛台线索（关键线索）
bool clueLetter = false;       // 信件遗嘱线索
bool clueScarf = false;        // 女士围巾线索（关键线索）
bool clueFootprint = false;    // 泥脚印线索
int keyClueCount = 0;          // 关键线索计数（需收集3条）

// 询问状态
bool wifeInterrogated = false;   // 是否询问过男爵夫人
bool jackInterrogated = false;   // 是否询问过养子杰克
bool butlerInterrogated = false; // 是否询问过老管家

void LoadImages() {
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    TCHAR* lastBackslash = _tcsrchr(exePath, _T('\\'));
    if (lastBackslash) *lastBackslash = _T('\0');
    TCHAR* secondBackslash = _tcsrchr(exePath, _T('\\'));
    if (secondBackslash) *secondBackslash = _T('\0');
    TCHAR* thirdBackslash = _tcsrchr(exePath, _T('\\'));
    if (thirdBackslash) *thirdBackslash = _T('\0');

    TCHAR imgPath[MAX_PATH];
    _stprintf_s(imgPath, MAX_PATH, _T("%s\\image.png"), exePath);

    TCHAR fullPath[MAX_PATH];
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\background.png"), imgPath);
    loadimage(&imgBackground, fullPath, WINDOW_WIDTH, WINDOW_HEIGHT);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\伯爵夫人.png"), imgPath);
    loadimage(&imgWife, fullPath, 180, 380);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\养子.png"), imgPath);
    loadimage(&imgJack, fullPath, 180, 380);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\管家.png"), imgPath);
    loadimage(&imgButler, fullPath, 180, 380);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\烛台.png"), imgPath);
    loadimage(&imgCandle, fullPath, 140, 140);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\信件和遗嘱草稿.png"), imgPath);
    loadimage(&imgLetter, fullPath, 100, 70);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\女士羊毛围巾.png"), imgPath);
    loadimage(&imgScarf, fullPath, 90, 180);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\泥脚印.png"), imgPath);
    loadimage(&imgFootprint, fullPath, 70, 50);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\男爵尸体.png"), imgPath);
    loadimage(&imgBody, fullPath, 150, 150);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\对话框.png"), imgPath);
    loadimage(&imgDialogBox, fullPath, 600, 350);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\卡片线索.png"), imgPath);
    loadimage(&imgClueCard, fullPath, 500, 300);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\标题.png"), imgPath);
    loadimage(&imgTitle, fullPath, 400, 150);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\指认按钮—手铐.png"), imgPath);
    loadimage(&imgAccuseBtn, fullPath, 180, 50);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\胜利结算.png"), imgPath);
    loadimage(&imgWinEnd, fullPath, WINDOW_WIDTH, WINDOW_HEIGHT);
    _stprintf_s(fullPath, MAX_PATH, _T("%s\\失败结算.png"), imgPath);
    loadimage(&imgLoseEnd, fullPath, WINDOW_WIDTH, WINDOW_HEIGHT);
}

// 绘制标题界面
void SetChineseFont(int height) {
    LOGFONT f = {0};
    f.lfHeight = height;
    f.lfWidth = 0;
    f.lfEscapement = 0;
    f.lfOrientation = 0;
    f.lfWeight = FW_NORMAL;
    f.lfItalic = FALSE;
    f.lfUnderline = FALSE;
    f.lfStrikeOut = FALSE;
    f.lfCharSet = GB2312_CHARSET;
    f.lfOutPrecision = OUT_DEFAULT_PRECIS;
    f.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    f.lfQuality = DEFAULT_QUALITY;
    f.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    _tcscpy(f.lfFaceName, _T("微软雅黑"));
    setfont(&f);
}

void DrawTitleScreen() {
    cleardevice();
    putimage(0, 0, &imgBackground);
    
    int titleX = (WINDOW_WIDTH - 400) / 2;
    putimage(titleX, 100, &imgTitle);
    
    setbkcolor(TRANSPARENT);
    SetChineseFont(30);
    settextcolor(RGB(200, 180, 100));
    outtextxy(450, 280, "点击任意处开始调查");
}

void DrawIntroScreen() {
    cleardevice();
    putimage(0, 0, &imgBackground);
    
    int boxX = (WINDOW_WIDTH - 600) / 2;
    putimage(boxX, 100, &imgDialogBox);
    
    setbkcolor(TRANSPARENT);
    SetChineseFont(24);
    settextcolor(RGB(220, 200, 150));
    outtextxy(boxX + 30, 130, _T("案发地点：郊外的灰石古堡"));
    outtextxy(boxX + 30, 180, _T("案发时间：月圆之夜，深夜11点"));
    outtextxy(boxX + 30, 230, _T("受害者：古堡主人 - 老男爵（70岁）"));
    outtextxy(boxX + 30, 280, _T("你是：路过古堡避雨的侦探"));
    outtextxy(boxX + 220, 380, _T("点击继续..."));
}

void DrawGameScreen() {
    cleardevice();
    putimage(0, 0, &imgBackground);
    
    setbkcolor(TRANSPARENT);
    SetChineseFont(35);
    settextcolor(RGB(255, 100, 100));
    outtextxy(450, 20, _T("古堡谜案 - 调查"));
    
    TCHAR clueText[200];
    _stprintf_s(clueText, 200, _T("关键线索: %d/3"), keyClueCount);
    SetChineseFont(24);
    settextcolor(RGB(200, 180, 100));
    outtextxy(50, 650, clueText);
    
    putimage(100, 420, &imgCandle);
    outtextxy(120, 570, _T("烛台(凶器)"));
    
    putimage(220, 450, &imgLetter);
    outtextxy(240, 530, _T("信件遗嘱"));
    
    putimage(550, 500, &imgScarf);
    outtextxy(560, 690, _T("女士围巾"));
    
    putimage(300, 580, &imgFootprint);
    outtextxy(310, 640, _T("泥脚印"));
    
    putimage(950, 180, &imgWife);
    outtextxy(970, 570, _T("男爵夫人"));
    
    putimage(950, 380, &imgJack);
    outtextxy(970, 770, _T("养子杰克"));
    
    putimage(1050, 380, &imgButler);
    outtextxy(1070, 770, _T("老管家"));
    
    putimage(WINDOW_WIDTH - 220, 620, &imgAccuseBtn);
}

void ShowClue(const TCHAR* title, const TCHAR* desc) {
    cleardevice();
    putimage(0, 0, &imgBackground);
    
    int cardX = (WINDOW_WIDTH - 500) / 2;
    putimage(cardX, 200, &imgClueCard);
    
    setbkcolor(TRANSPARENT);
    SetChineseFont(30);
    settextcolor(RGB(255, 220, 100));
    outtextxy(cardX + 30, 220, title);
    
    SetChineseFont(22);
    settextcolor(RGB(220, 200, 160));
    outtextxy(cardX + 30, 280, desc);
    
    SetChineseFont(20);
    settextcolor(RGB(255, 255, 200));
    outtextxy(cardX + 160, 410, _T("【点击继续】"));
    
    FlushBatchDraw();
    
    bool waiting = true;
    while (waiting) {
        Sleep(30);
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                waiting = false;
            }
        }
    }
}

void DrawQuestionScreen() {
    cleardevice();
    putimage(0, 0, &imgBackground);
    
    setbkcolor(TRANSPARENT);
    SetChineseFont(35);
    settextcolor(RGB(255, 100, 100));
    outtextxy(450, 20, _T("古堡谜案 - 询问"));
    
    int x = 100;
    if (currentSuspect == SUSPECT_WIFE) {
        putimage(x, 100, &imgWife);
        SetChineseFont(28);
        settextcolor(RGB(255, 220, 100));
        outtextxy(480, 150, _T("男爵夫人（45岁）"));
        
        SetChineseFont(22);
        settextcolor(RGB(200, 200, 180));
        outtextxy(480, 210, _T("关系：老男爵的第二任妻子"));
        outtextxy(480, 260, _T("动机：老男爵准备修改遗嘱"));
        outtextxy(480, 310, _T("证词：我在卧室看书，没听到声音"));
    } else if (currentSuspect == SUSPECT_JACK) {
        putimage(x, 100, &imgJack);
        SetChineseFont(28);
        settextcolor(RGB(255, 220, 100));
        outtextxy(480, 150, _T("养子杰克（28岁）"));
        
        SetChineseFont(22);
        settextcolor(RGB(200, 200, 180));
        outtextxy(480, 210, _T("关系：老男爵收养的儿子"));
        outtextxy(480, 260, _T("动机：经营不善亏了很多钱"));
        outtextxy(480, 310, _T("证词：我承认和男爵吵架了"));
    } else if (currentSuspect == SUSPECT_BUTLER) {
        putimage(x, 100, &imgButler);
        SetChineseFont(28);
        settextcolor(RGB(255, 220, 100));
        outtextxy(480, 150, _T("老管家（60岁）"));
        
        SetChineseFont(22);
        settextcolor(RGB(200, 200, 180));
        outtextxy(480, 210, _T("关系：在古堡工作30年"));
        outtextxy(480, 260, _T("动机：女儿生病需要钱"));
        outtextxy(480, 310, _T("证词：我看到杰克从书房出来"));
    }
    
    SetChineseFont(24);
    settextcolor(RGB(255, 255, 200));
    outtextxy(WINDOW_WIDTH - 200, 650, _T("返回调查"));
}

void DrawAccuseScreen() {
    cleardevice();
    putimage(0, 0, &imgBackground);
    
    setbkcolor(TRANSPARENT);
    SetChineseFont(40);
    settextcolor(RGB(255, 80, 80));
    outtextxy(500, 50, _T("指认真凶"));
    
    TCHAR clueText[100];
    _stprintf_s(clueText, 100, _T("已收集关键线索: %d/3"), keyClueCount);
    SetChineseFont(25);
    settextcolor(RGB(255, 220, 100));
    outtextxy(500, 130, clueText);
    
    putimage(150, 220, &imgWife);
    putimage(540, 220, &imgJack);
    putimage(930, 220, &imgButler);
    
    SetChineseFont(22);
    settextcolor(RGB(255, 255, 255));
    outtextxy(190, 610, _T("男爵夫人"));
    outtextxy(580, 610, _T("养子杰克"));
    outtextxy(970, 610, _T("老管家"));
    
    outtextxy(WINDOW_WIDTH - 200, 650, _T("返回调查"));
}

void DrawEndingScreen(bool isWin) {
    cleardevice();
    setbkcolor(TRANSPARENT);
    
    if (isWin) {
        putimage(0, 0, &imgWinEnd);
        SetChineseFont(50);
        settextcolor(RGB(255, 220, 100));
        outtextxy(400, 100, _T("完美结案"));
        
        SetChineseFont(25);
        settextcolor(RGB(220, 200, 150));
        outtextxy(200, 200, _T("在铁证面前，老管家终于承认了一切！"));
        outtextxy(200, 250, _T("女儿生病需要钱，男爵拒绝帮助..."));
        outtextxy(200, 300, _T("他趁机用烛台打死了老男爵"));
        outtextxy(200, 350, _T("管家被逮捕归案，案件终于水落石出！"));
    } else {
        putimage(0, 0, &imgLoseEnd);
        SetChineseFont(50);
        settextcolor(RGB(200, 80, 80));
        outtextxy(450, 150, _T("指控错误"));
        
        SetChineseFont(28);
        settextcolor(RGB(200, 180, 150));
        outtextxy(250, 280, _T("真凶依然在逃，案件成为悬案..."));
        outtextxy(250, 330, _T("提示：需要收集至少3条关键线索！"));
    }
    
    SetChineseFont(24);
    settextcolor(RGB(255, 255, 200));
    outtextxy(450, 600, _T("【点击任意处重新开始】"));
}

bool IsPointInRect(int x, int y, int rx, int ry, int rw, int rh) {
    return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

void HandleMouseClick(int x, int y) {
    switch (currentState) {
        case STATE_TITLE:
            currentState = STATE_INTRO;
            break;
        case STATE_INTRO:
            currentState = STATE_GAME;
            break;
        case STATE_GAME:
            if (IsPointInRect(x, y, 80, 200, 150, 150)) {
                if (!clueCandle) {
                    clueCandle = true;
                    keyClueCount++;
                }
                ShowClue(_T("烛台（凶器）"), clueCandle ? 
                    _T("烛台上有管家的指纹！") : _T("烛台是凶器！"));
            } else if (IsPointInRect(x, y, 280, 250, 120, 80)) {
                if (!clueLetter) clueLetter = true;
                ShowClue(_T("信件遗嘱"), _T("男爵准备修改遗嘱！"));
            } else if (IsPointInRect(x, y, 500, 180, 100, 200)) {
                if (!clueScarf) {
                    clueScarf = true;
                    keyClueCount++;
                }
                ShowClue(_T("女士围巾"), _T("是男爵夫人的物品！"));
            } else if (IsPointInRect(x, y, 750, 300, 80, 60)) {
                if (!clueFootprint) clueFootprint = true;
                ShowClue(_T("泥脚印"), _T("泥脚印从大门到书房！"));
            } else if (IsPointInRect(x, y, 950, 180, 200, 400)) {
                currentSuspect = SUSPECT_WIFE;
                currentState = STATE_QUESTION;
            } else if (IsPointInRect(x, y, 950, 350, 200, 400)) {
                currentSuspect = SUSPECT_JACK;
                currentState = STATE_QUESTION;
            } else if (IsPointInRect(x, y, 1050, 350, 200, 400)) {
                currentSuspect = SUSPECT_BUTLER;
                currentState = STATE_QUESTION;
            } else if (IsPointInRect(x, y, WINDOW_WIDTH - 220, 500, 180, 60)) {
                currentState = STATE_ACCUSE;
            }
            break;
        case STATE_QUESTION:
            if (IsPointInRect(x, y, WINDOW_WIDTH - 200, 650, 180, 40)) {
                currentState = STATE_GAME;
            }
            break;
        case STATE_ACCUSE:
            if (IsPointInRect(x, y, 150, 220, 200, 400)) {
                currentState = STATE_ENDING_LOSE;
            } else if (IsPointInRect(x, y, 540, 220, 200, 400)) {
                currentState = STATE_ENDING_LOSE;
            } else if (IsPointInRect(x, y, 930, 220, 200, 400)) {
                if (keyClueCount >= 3) {
                    currentState = STATE_ENDING_WIN;
                } else {
                    currentState = STATE_ENDING_LOSE;
                }
            } else if (IsPointInRect(x, y, WINDOW_WIDTH - 200, 650, 180, 40)) {
                currentState = STATE_GAME;
            }
            break;
        case STATE_ENDING_WIN:
        case STATE_ENDING_LOSE:
            currentState = STATE_TITLE;
            clueCandle = false;
            clueLetter = false;
            clueScarf = false;
            clueFootprint = false;
            keyClueCount = 0;
            wifeInterrogated = false;
            jackInterrogated = false;
            butlerInterrogated = false;
            break;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    LoadImages();
    BeginBatchDraw();
    
    while (true) {
        switch (currentState) {
            case STATE_TITLE:
                DrawTitleScreen();
                break;
            case STATE_INTRO:
                DrawIntroScreen();
                break;
            case STATE_GAME:
                DrawGameScreen();
                break;
            case STATE_QUESTION:
                DrawQuestionScreen();
                break;
            case STATE_ACCUSE:
                DrawAccuseScreen();
                break;
            case STATE_ENDING_WIN:
                DrawEndingScreen(true);
                break;
            case STATE_ENDING_LOSE:
                DrawEndingScreen(false);
                break;
        }
        
        FlushBatchDraw();
        Sleep(30);
        
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                HandleMouseClick(msg.x, msg.y);
            }
        }
    }
    
    EndBatchDraw();
    closegraph();
    return 0;
}
