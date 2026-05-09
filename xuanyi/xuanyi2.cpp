/*
 * 古宅谜案 - 悬疑推理探案游戏（稳定版）
 * 环境：Visual Studio + EasyX 图形库
 * 字符集要求：多字节字符集
 * 游戏玩法：玩家扮演侦探哈德逊，调查古宅谋杀案，收集线索后指认真凶
 */

#include <graphics.h>
#include <conio.h>
#include <vector>
#include <string>
#include <algorithm>

// 窗口尺寸常量
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 700;

// 按钮结构体：定义按钮的坐标范围和显示文本
struct Button {
    int x1, y1, x2, y2;   // 按钮矩形区域的左上角和右下角坐标
    std::string text;      // 按钮显示文本
};

// 游戏状态变量
bool gameOver = false;       // 游戏是否结束
bool gameWin = false;        // 是否获胜
bool identifying = false;    // 是否处于指认凶手状态

// 游戏数据
std::vector<std::string> clues;    // 已收集的线索列表
std::vector<std::string> messages; // 调查记录消息列表

// 调查状态标记
bool hasExamined = false;      // 是否勘查过现场
bool hasTalkedButler = false;  // 是否询问过管家
bool hasTalkedWife = false;    // 是否询问过夫人
bool hasTalkedSon = false;     // 是否询问过儿子
bool hasTalkedMaid = false;    // 是否询问过女仆

// 关键线索标记（用于判断能否成功指认）
bool clueWindowFake = false;   // 窗户伪造线索
bool clueWillChanged = false;  // 遗嘱篡改线索
bool clueMaidWitness = false;  // 女仆目击线索

// 功能按钮定义（右侧操作区）
Button btnInvestigate = { 780, 100, 950, 140, "[现场勘查]" };
Button btnButler = { 780, 160, 950, 200, "[询问管家]" };
Button btnWife = { 780, 220, 950, 260, "[询问夫人]" };
Button btnSon = { 780, 280, 950, 320, "[询问儿子]" };
Button btnMaid = { 780, 340, 950, 380, "[询问女仆]" };
Button btnAccuse = { 780, 410, 950, 450, "[指认真凶]" };
Button btnReset = { 780, 630, 950, 670, "[重新开始]" };

// 指认界面按钮定义
Button btnAccuseButler = { 300, 250, 480, 290, "指认管家" };
Button btnAccuseWife = { 520, 250, 700, 290, "指认夫人" };
Button btnAccuseSon = { 300, 320, 480, 360, "指认儿子" };
Button btnAccuseMaid = { 520, 320, 700, 360, "指认女仆" };
Button btnCancelAccuse = { 380, 400, 620, 440, "取消指认" };

// 函数声明
void InitGame();                      // 初始化游戏状态
void AddMessage(const std::string& msg); // 添加调查消息
void AddClue(const std::string& clue, bool isKeyClue = false); // 添加线索
void DrawUI();                        // 绘制游戏界面
void ProcessMouse(int x, int y);      // 处理鼠标点击（主界面）
void ProcessAccuse(int x, int y);     // 处理指认界面鼠标点击
void CheckEnding(bool correct);       // 处理游戏结局
void ResetGame();                     // 重置游戏

// 初始化游戏状态：重置所有变量并显示案件简报
void InitGame() {
    gameOver = false;
    gameWin = false;
    identifying = false;
    clues.clear();
    messages.clear();
    hasExamined = false;
    hasTalkedButler = false;
    hasTalkedWife = false;
    hasTalkedSon = false;
    hasTalkedMaid = false;
    clueWindowFake = false;
    clueWillChanged = false;
    clueMaidWitness = false;

    // 显示案件简报
    AddMessage("[案件简报]");
    AddMessage("富豪詹姆斯被发现死在书房，后脑遭到重击。");
    AddMessage("你是侦探哈德逊，请调查古宅并找出真凶。");
    AddMessage("使用右侧按钮调查取证，收集足够线索后指认真凶。");
    AddMessage("----------------------------");
}

// 添加调查消息到消息列表
// @param msg: 要添加的消息内容
void AddMessage(const std::string& msg) {
    messages.push_back(msg);
    if (messages.size() > 20) messages.erase(messages.begin()); // 保持最多20条消息
}

// 添加线索到线索列表
// @param clue: 线索内容
// @param isKeyClue: 是否为关键线索
void AddClue(const std::string& clue, bool isKeyClue) {
    if (std::find(clues.begin(), clues.end(), clue) != clues.end()) return; // 避免重复添加
    clues.push_back(clue);
    AddMessage("[新线索]" + clue);
    // 如果是关键线索，更新对应的标记
    if (isKeyClue) {
        if (clue.find("窗户伪造") != std::string::npos) clueWindowFake = true;
        if (clue.find("遗嘱篡改") != std::string::npos) clueWillChanged = true;
        if (clue.find("女仆目击") != std::string::npos) clueMaidWitness = true;
    }
}

// 绘制游戏主界面
void DrawUI() {
    cleardevice();
    setbkcolor(WHITE);
    setfillcolor(LIGHTGRAY);
    fillrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // 绘制标题和案件信息
    settextcolor(BLACK);
    settextstyle(30, 0, "宋体");
    outtextxy(30, 20, "《古宅谜案》——悬疑推理探案");
    settextstyle(16, 0, "宋体");
    outtextxy(30, 65, "受害者：詹姆斯·温特（书房内发现，后脑遭钝器重击）");
    outtextxy(30, 90, "嫌疑人：管家、夫人、儿子、女仆");

    // 绘制线索列表区域
    setfillcolor(RGB(230, 240, 250));
    fillrectangle(20, 120, 750, 300);
    settextcolor(DARKGRAY);
    outtextxy(30, 130, "已有线索列表:");
    int lineY = 160;
    for (size_t i = 0; i < clues.size() && i < 12; i++) {
        std::string showClue = "  " + clues[i];
        if (lineY > 280) break;
        outtextxy(35, lineY, showClue.c_str());
        lineY += 24;
    }
    if (clues.empty()) outtextxy(35, 160, "（暂无线索，请开始调查）");

    // 绘制调查记录区域
    setfillcolor(RGB(250, 245, 235));
    fillrectangle(20, 320, 750, 680);
    settextcolor(BLACK);
    outtextxy(30, 330, "调查记录:");
    int logY = 360;
    int start = (int)messages.size() > 14 ? messages.size() - 14 : 0;
    for (int i = start; i < (int)messages.size(); i++) {
        outtextxy(35, logY, messages[i].c_str());
        logY += 24;
        if (logY > 660) break;
    }

    // 绘制功能按钮（右侧操作区）
    settextstyle(20, 0, "宋体");
    setfillcolor(RGB(200, 220, 200));
    fillrectangle(btnInvestigate.x1, btnInvestigate.y1, btnInvestigate.x2, btnInvestigate.y2);
    outtextxy(btnInvestigate.x1 + 10, btnInvestigate.y1 + 5, btnInvestigate.text.c_str());

    setfillcolor(RGB(220, 200, 200));
    fillrectangle(btnButler.x1, btnButler.y1, btnButler.x2, btnButler.y2);
    outtextxy(btnButler.x1 + 10, btnButler.y1 + 5, btnButler.text.c_str());
    fillrectangle(btnWife.x1, btnWife.y1, btnWife.x2, btnWife.y2);
    outtextxy(btnWife.x1 + 10, btnWife.y1 + 5, btnWife.text.c_str());
    fillrectangle(btnSon.x1, btnSon.y1, btnSon.x2, btnSon.y2);
    outtextxy(btnSon.x1 + 10, btnSon.y1 + 5, btnSon.text.c_str());
    fillrectangle(btnMaid.x1, btnMaid.y1, btnMaid.x2, btnMaid.y2);
    outtextxy(btnMaid.x1 + 10, btnMaid.y1 + 5, btnMaid.text.c_str());
    
    setfillcolor(RGB(220, 220, 250));
    fillrectangle(btnAccuse.x1, btnAccuse.y1, btnAccuse.x2, btnAccuse.y2);
    outtextxy(btnAccuse.x1 + 10, btnAccuse.y1 + 5, btnAccuse.text.c_str());
    
    setfillcolor(RGB(200, 200, 200));
    fillrectangle(btnReset.x1, btnReset.y1, btnReset.x2, btnReset.y2);
    outtextxy(btnReset.x1 + 10, btnReset.y1 + 5, btnReset.text.c_str());

    // 绘制游戏结束提示
    if (gameOver) {
        settextcolor(RGB(180, 0, 0));
        settextstyle(35, 0, "黑体");
        const char* endMsg = gameWin ? "[恭喜] 真凶伏法！" : "[失败] 真凶逍遥法外...";
        outtextxy(300, 550, endMsg);
        settextstyle(20, 0, "宋体");
        outtextxy(280, 610, "点击「重新开始」继续挑战");
    }

    // 绘制指认界面（弹出式对话框）
    if (identifying && !gameOver) {
        setfillcolor(ARGB(180, 0, 0, 0));  // 半透明遮罩
        solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        setfillcolor(RGB(240, 240, 210));
        fillrectangle(260, 200, 740, 500);
        settextcolor(BLACK);
        settextstyle(24, 0, "宋体");
        outtextxy(340, 210, "选择你要指控的凶手");

        // 绘制嫌疑人按钮
        setfillcolor(RGB(220, 180, 180));
        fillrectangle(btnAccuseButler.x1, btnAccuseButler.y1, btnAccuseButler.x2, btnAccuseButler.y2);
        outtextxy(btnAccuseButler.x1 + 20, btnAccuseButler.y1 + 5, btnAccuseButler.text.c_str());
        fillrectangle(btnAccuseWife.x1, btnAccuseWife.y1, btnAccuseWife.x2, btnAccuseWife.y2);
        outtextxy(btnAccuseWife.x1 + 20, btnAccuseWife.y1 + 5, btnAccuseWife.text.c_str());
        fillrectangle(btnAccuseSon.x1, btnAccuseSon.y1, btnAccuseSon.x2, btnAccuseSon.y2);
        outtextxy(btnAccuseSon.x1 + 20, btnAccuseSon.y1 + 5, btnAccuseSon.text.c_str());
        fillrectangle(btnAccuseMaid.x1, btnAccuseMaid.y1, btnAccuseMaid.x2, btnAccuseMaid.y2);
        outtextxy(btnAccuseMaid.x1 + 20, btnAccuseMaid.y1 + 5, btnAccuseMaid.text.c_str());
        
        // 绘制取消按钮
        setfillcolor(RGB(180, 180, 220));
        fillrectangle(btnCancelAccuse.x1, btnCancelAccuse.y1, btnCancelAccuse.x2, btnCancelAccuse.y2);
        outtextxy(btnCancelAccuse.x1 + 30, btnCancelAccuse.y1 + 5, btnCancelAccuse.text.c_str());
    }
}

// 处理主界面鼠标点击事件
// @param x, y: 鼠标点击坐标
void ProcessMouse(int x, int y) {
    if (gameOver) {
        if (x >= btnReset.x1 && x <= btnReset.x2 && y >= btnReset.y1 && y <= btnReset.y2) ResetGame();
        return;
    }
    if (identifying) return;  // 指认界面时不处理主界面点击

    // 处理现场勘查按钮
    if (x >= btnInvestigate.x1 && x <= btnInvestigate.x2 && y >= btnInvestigate.y1 && y <= btnInvestigate.y2) {
        if (!hasExamined) {
            hasExamined = true;
            AddMessage("你仔细勘查了书房现场...");
            AddClue("凶器是青铜烛台（在壁炉旁发现）", false);
            AddClue("[关键]窗户撬锁痕迹为伪造：锁具完好，外侧撬痕是新做的", true);
            AddClue("[关键]书桌暗格内发现遗嘱修改草稿，儿子原本继承大部分遗产被削减", true);
            AddMessage("现场提取到模糊脚印和烛台指纹。");
        }
        else AddMessage("现场已经勘查过了，没有更多线索。");
        return;
    }
    
    // 处理询问管家按钮
    if (x >= btnButler.x1 && x <= btnButler.x2 && y >= btnButler.y1 && y <= btnButler.y2) {
        if (!hasTalkedButler) {
            hasTalkedButler = true;
            AddMessage("老管家神情紧张地说：");
            AddMessage("「晚上9点左右，我听到老爷和儿子在书房激烈争吵，");
            AddMessage("  之后不久传来一声闷响……我不敢进去查看。」");
            AddClue("管家证词：案发时段儿子与死者争吵，随后有可疑声响", false);
        }
        else AddMessage("管家再次重复证词，没有新的信息。");
        return;
    }
    
    // 处理询问夫人按钮
    if (x >= btnWife.x1 && x <= btnWife.x2 && y >= btnWife.y1 && y <= btnWife.y2) {
        if (!hasTalkedWife) {
            hasTalkedWife = true;
            AddMessage("夫人强忍泪水：");
            AddMessage("「詹姆斯最近和儿子埃德温为了公司继承权闹翻了，");
            AddMessage("  埃德温甚至威胁要让他父亲后悔……我真不敢相信。」");
            AddClue("夫人透露：儿子与被害者有严重财务矛盾，存在强烈动机", false);
        }
        else AddMessage("夫人不愿再多说，只是默默哭泣。");
        return;
    }
    
    // 处理询问儿子按钮
    if (x >= btnSon.x1 && x <= btnSon.x2 && y >= btnSon.y1 && y <= btnSon.y2) {
        if (!hasTalkedSon) {
            hasTalkedSon = true;
            AddMessage("埃德温暴躁地反驳：");
            AddMessage("「我虽然恨他削减我的遗产，但人不是我杀的！");
            AddMessage("  我一直在卧室看书！」但他的眼神躲闪闪烁。");
            AddClue("儿子案发时间声称在卧室，但无目击证人，行迹可疑", false);
        }
        else AddMessage("儿子拒绝回答任何问题：「找我的律师谈！」");
        return;
    }
    
    // 处理询问女仆按钮
    if (x >= btnMaid.x1 && x <= btnMaid.x2 && y >= btnMaid.y1 && y <= btnMaid.y2) {
        if (!hasTalkedMaid) {
            hasTalkedMaid = true;
            AddMessage("年轻女仆瑟瑟发抖：");
            AddMessage("「大约9点多，我去打扫走廊时，看见少爷从书房方向出来，");
            AddMessage("  他的袖口好像有暗色污渍，看到我就快步走开了。」");
            AddClue("[关键]女仆目击：案发后不久儿子从书房方向走出，袖口有疑似血迹", true);
        }
        else AddMessage("女仆害怕地说：「我已经把知道的都告诉你了。」");
        return;
    }
    
    // 处理指认真凶按钮
    if (x >= btnAccuse.x1 && x <= btnAccuse.x2 && y >= btnAccuse.y1 && y <= btnAccuse.y2) {
        if (!gameOver) identifying = true;
        return;
    }
    
    // 处理重新开始按钮
    if (x >= btnReset.x1 && x <= btnReset.x2 && y >= btnReset.y1 && y <= btnReset.y2) ResetGame();
}

// 处理指认界面鼠标点击事件
// @param x, y: 鼠标点击坐标
void ProcessAccuse(int x, int y) {
    if (!identifying || gameOver) return;
    
    // 取消指认
    if (x >= btnCancelAccuse.x1 && x <= btnCancelAccuse.x2 && y >= btnCancelAccuse.y1 && y <= btnCancelAccuse.y2) {
        identifying = false;
        AddMessage("你暂时中止了指认，继续调查。");
        return;
    }
    
    // 指控管家（错误）
    if (x >= btnAccuseButler.x1 && x <= btnAccuseButler.x2 && y >= btnAccuseButler.y1 && y <= btnAccuseButler.y2) {
        identifying = false;
        AddMessage("你当众指控管家是凶手！");
        AddMessage("但管家有不在场证明，且缺乏动机，最终证明是冤枉。");
        CheckEnding(false);
        return;
    }
    
    // 指控夫人（错误）
    if (x >= btnAccuseWife.x1 && x <= btnAccuseWife.x2 && y >= btnAccuseWife.y1 && y <= btnAccuseWife.y2) {
        identifying = false;
        AddMessage("你指控夫人谋杀了自己的丈夫。");
        AddMessage("夫人虽然与死者有矛盾，但所有证据指向他人，夫人无罪释放。");
        CheckEnding(false);
        return;
    }
    
    // 指控儿子（正确答案）
    if (x >= btnAccuseSon.x1 && x <= btnAccuseSon.x2 && y >= btnAccuseSon.y1 && y <= btnAccuseSon.y2) {
        identifying = false;
        AddMessage("你冷静地指出埃德温才是真凶！");
        int keyCount = clueWindowFake + clueWillChanged + clueMaidWitness;
        if (keyCount >= 2) {
            // 收集到足够关键线索，成功破案
            AddMessage("由于你收集到了窗户伪造痕迹、遗嘱篡改证据以及女仆目击证词，");
            AddMessage("再加上动机与时间线吻合，埃德温无话可说，最终认罪！");
            CheckEnding(true);
        }
        else {
            // 线索不足，无法定罪
            AddMessage("虽然你的直觉是正确的，但手中的证据不足，");
            AddMessage("陪审团因证据链不完整而无法定罪，案件成为悬案。");
            CheckEnding(false);
        }
        return;
    }
    
    // 指控女仆（错误）
    if (x >= btnAccuseMaid.x1 && x <= btnAccuseMaid.x2 && y >= btnAccuseMaid.y1 && y <= btnAccuseMaid.y2) {
        identifying = false;
        AddMessage("你指控柔弱的玛莎是真凶。");
        AddMessage("玛莎哭着说当时去给花浇水，目击者们也证明她手无缚鸡之力。");
        CheckEnding(false);
        return;
    }
}

// 处理游戏结局
// @param correct: 是否正确指认凶手
void CheckEnding(bool correct) {
    gameOver = true;
    gameWin = correct;
    if (gameWin) {
        AddMessage("========== 完美结案 ==========");
        AddMessage("你凭借缜密的推理揪出了真凶，名声大噪！");
        AddMessage("真相：埃德温因遗产被削减，伪造入室盗窃并杀害父亲。");
    }
    else {
        AddMessage("========== 案件未破 ==========");
        AddMessage("真凶依然逍遥法外，你深感挫败……");
        AddMessage("或许下次需要更仔细地搜集线索。");
    }
}

// 重置游戏状态
void ResetGame() {
    InitGame();
    gameOver = false;
    gameWin = false;
    identifying = false;
}

// 主函数：游戏入口
int main() {
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(WHITE);
    InitGame();
    
    EXMSG msg;
    bool running = true;
    
    // 游戏主循环
    while (running) {
        DrawUI();
        
        // 处理消息队列
        if (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                int mx = msg.x, my = msg.y;
                if (identifying && !gameOver) ProcessAccuse(mx, my);
                else ProcessMouse(mx, my);
            }
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) running = false;
            if (msg.message == WM_CLOSE) running = false;
        }
        Sleep(30);
    }
    
    closegraph();
    return 0;
}