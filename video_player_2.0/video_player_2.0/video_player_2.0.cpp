//../../test_video/失眠飞行.mp4
#include "x_player.h"

int main(int argc, char* argv[])
{
    SetProcessDPIAware();

    xM::xPlayer player;
    //if (true == player.Initialize("../../videos/连名带姓-黄霄云.mp4"))
    if (true == player.Initialize("../../videos/连名带姓-黄霄云.mp4"))
        player.Work();
    return 0;
}