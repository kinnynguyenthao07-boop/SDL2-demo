#include "mylib.h"
#include <iostream>
#include <conio.h>
#include <string>
#include <ctime>
#include <windows.h>
#define sl_ground 90
using namespace std;
//=====Khai báo biến
//-- Biến khủng long
int h = 6; // height
int xdino = 25;
int ydino = 17;
int xdat = 10, ydat = ydino + h;
//--Biến đất
char ground[3][sl_ground + 1];
//--Biến nhảy
bool Jump = false; //nhay
bool Fall = false; //roi
bool Duck = false; //cui
int jump_height = 10;
int jumpY = 0;
bool kt_jump = false;
//--Biến cây
void draw_xrong(int x);

//--Biến kiểm tra va chạm
bool kt_cham(int xxrong , int xmui , int ymui , int x , int y );//xmui, ymui là tọa độ mũi của khủng long
int xmui = xdino + 13; int ymui = ydino + 1;
//--Biến khác
bool game_over (int x , int y , int t[] , int nt);//nt là số cây


//---Vẽ dino
string dino[6] = {
    "        ██▀█▄    \n",
    "        ███▬     \n",
    "       ████▬.\n",
    "     ▄█████\n",
    "  ▬▄█████▀\n",
    "     ▀▄▀▄\n"
};
string space = "        ";
string die = "         ████▄   ";

//---- 3 loại xương rồng 
string xrong1[2] =
{ 
    "▄ █ ▄",
    "▀███▀"
};
string xrong2[1] =
{
    "▄█▄"
};
string xrong3 [2] =
{
    " █▀█▄",
    "█▀ █ "
};

void play();
//--Khủng long
void draw_dino(int x, int y)
{
    y = ydino - jumpY;
    for (int i = 0; i < 6; i++)
    {
        gotoXY(x, y + i);
        cout << dino[i];
    
    }
}
void clear_dino(int x, int y)
{
    for (int i = 0; i < 6; i++)
    {
        gotoXY(x, y + i);
        cout << "                    "; // xóa một vùng đủ lớn
    }
}
//--Đất

void draw_mat_dat()
{
    if (kt_jump == false)
    {
        ground[0][xdino - xdat] ='.';
        ground[0][xdino + 1 - xdat] ='|';
        ground[0][xdino + 2 - xdat] ='_';
        ground[0][xdino + 3 - xdat] ='|';
        ground[0][xdino + 4 - xdat] ='-';
        ground[0][xdino + 5 - xdat] ='|';
        ground[0][xdino + 6 - xdat] ='_';
        ground[0][xdino + 7 - xdat] ='|';
        ground[0][xdino + 8 - xdat] ='_';
        ground[0][xdino + 9 - xdat] ='_';
        ground[0][xdino + 10 - xdat] ='_';
        ground[0][xdino + 11 - xdat] ='_';
        ground[0][xdino + 12 - xdat] ='_';
    } else {
        ground[0][xdino - xdat] ='_';
        ground[0][xdino + 1 - xdat] ='_';
        ground[0][xdino + 2 - xdat] ='_';
        ground[0][xdino + 3 - xdat] ='_';
        ground[0][xdino + 4 - xdat] ='_';
        ground[0][xdino + 5 - xdat] ='_';
        ground[0][xdino + 6 - xdat] ='_';
        ground[0][xdino + 7 - xdat] ='_';
    }
    for (int i = 0 ; i < 3 ; i++) {
        gotoXY(xdat , ydat + i);
        cout << ground[i];
    }
}
void tao_dat(int i)
{
    int v = rand() % 6 + 1;
    switch (v)
    {
    case 1:
        ground[1][i] = '-';
        ground[2][i] = ' ';
        break;
    case 2:
        ground[1][i] = '.';
        ground[2][i] = ' ';
        break;
    case 3:
        ground[1][i] = '`';
        ground[2][i] = ' ';
        break;
    case 4:
        ground[1][i] = ' ';
        ground[2][i] = 248;
        break;
    case 5:
        ground[1][i] = 248;
        ground[2][i] = '`';
        break;
    case 6:
        ground[1][i] = ',';
        ground[2][i] = ':';
        break;
    default:
        ground[1][i] = ' ';
        ground[2][i] = ' ';
        break;
    }
}
void khoi_tao_mat_dat()
{
    for (int i = 0; i < sl_ground; i++)
    {
        ground[0][i] = '_';
        tao_dat(i);
    }
    ground[0][sl_ground] = '\0';
    ground[1][sl_ground] = '\0';
    ground[2][sl_ground] = '\0';
}
void di_chuyen_mat_dat()
{
    for (int i = 0; i < sl_ground - 1; i++)
    {
        ground[1][i] = ground[1][i + 1];
        ground[2][i] = ground[2][i + 1];
    }

    tao_dat(sl_ground - 1);
    draw_mat_dat();
}

//-- Cây 
void clear_xrong(int x)
{
    int y = ydat - 3;
    int len = (int)xrong1[0].length(); // chiều ngang của cây (ở đây ~7)
    string blanks(len, ' ');
    for (int i = 0; i < 2; i++)
    {
        gotoXY(x, y + i);
        cout << blanks;
    }
}
void di_chuyen_xrong(int t[] , int &nt )
{
    if (nt <= 0) return;

    int width = (int)xrong1[0].length();

    // 1) Xóa tất cả cây ở vị trí cũ (nếu nằm trong vùng hiển thị)
    for (int i = 0; i < nt; i++) {
        if (t[i] >= xdat - width && t[i] <= xdat + sl_ground) {
            clear_xrong(t[i]);
        }
    }

    // 2) Dịch trái tất cả tọa độ cây
    for (int i = 0; i < nt; i++) {
        t[i]--;
    }

    // 3) Loại bỏ cây đã rời hẳn khỏi vùng hiển thị (bên trái)
    int write = 0;
    for (int i = 0; i < nt; i++) {
        // giữ lại nếu phần phải của cây vẫn còn > xdat
        if (t[i] + width > xdat) {
            t[write++] = t[i];
        }
        // nếu không giữ (đã ra khỏi), thì vị trí cũ đã được xóa ở bước 1
    }
    nt = write;

    // 4) Vẽ tất cả cây còn lại ở vị trí mới (nếu nằm trong vùng hiển thị)
    for (int i = 0; i < nt; i++) {
        if (t[i] >= xdat && t[i] <= xdat + sl_ground) {
            draw_xrong(t[i]);
        }
    }
}
void draw_xrong(int x)
{
    int y = ydat - 3;
    for (int i=0 ; i < 2  ; i++) {
        gotoXY(x , y + i);
        cout << xrong1[i];
    }
}

//xóa cây trên màn hình
void xoa(int a[], int &n , int vt);//xóa tọa độ x của cây trong mảng để giảm gánh nặng dữ liệu
    //vt là vị trí cần xóa trong mảng
// ---Kiểm tra va chạm    
bool kt_cham(int xxrong , int xmui , int ymui , int x , int y )
{
    int xchan_trai = x; 
    int xchan_phai = x + 13;
    int ychan_tren = y; 
    int ychan_duoi = y + h;

    int xxrong_max = xxrong + 7;
    int yxrong = ydat - 3;
    int yxrong_max = ydat;

    xmui = x + 13; // x mũi luôn cập nhật theo vị trí mới
    //ymui = y + 1;  // y mũi luôn cập nhật theo vị trí mới
    ymui = y + 1;
    // kiểm tra giao nhau theo trục X
    bool chamX = 
    (
        (xchan_phai >= xxrong && xchan_trai <= xxrong_max) ||
        (xmui >= xxrong && xmui <= xxrong_max && ymui == yxrong)
    );

    // kiểm tra giao nhau theo trục Y (chân dino thấp hơn ngọn cây)
    bool chamY = (ychan_duoi >= yxrong);

    if (chamX && chamY)
        return true;
    return false;
}
/*bool kt_cham(int xxrong , int xmui , int ymui , int x , int y )
{
    // Chiều rộng thực tế khủng long ~ 13 ký tự
    int xchan_trai = x;
    int xchan_phai = x + 13; 

    int ychan_tren = y; 
    int ychan_duoi = y + h; // chiều cao khủng long (6 dòng)

    // Cây (xương rồng)
    int width_xr = (int)xrong1[0].length(); // ~5 ký tự
    int xxrong_max = xxrong + width_xr; 
    int yxrong = ydat - 3;
    int yxrong_max = ydat;

    // Cập nhật mũi theo vị trí thật của khủng long
    xmui = xchan_phai; 
    ymui = y + 1; 

    // Kiểm tra giao nhau theo X
    bool chamX = (xchan_phai >= xxrong && xchan_trai <= xxrong_max);

    // Kiểm tra theo Y (khi chân khủng long thấp hơn đỉnh cây)
    bool chamY = (ychan_duoi >= yxrong);

    return (chamX && chamY);
};*/

void xoa(int a[], int &n , int vt)
{
    for (int i = vt ; i < n - 1 ; i++) 
    {
        a[i] = a[i + 1];
    }
    n--;
}


/// --- Báo kết thúc
bool game_over (int x , int y , int t[] , int nt)
{
    for (int i = 0 ; i < nt ; i++) 
    {
        if (kt_cham(t[i], xmui , ymui , x , y) == true) 
        {
            gotoXY(x , y + 1);
            cout << die;
            gotoXY(48 , 10);
            cout << "GAME OVER!";
            return true;
        }
    }
    return false;
}

int main()
{  
    //can de doc ma ASCII
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("chcp 65001 >nul");

    //srand(time(0));
    ShowCur(0);
    play();
    _getch();
    return 0;
}

void play()
{
    int t[100]; int nt = 0; int count = 0;//nt là số cây; count là để giãn cách cây
    srand((unsigned) time (nullptr));
    khoi_tao_mat_dat();
    draw_mat_dat();
    di_chuyen_mat_dat();
    int x =xdino, y = ydino;
    int check = !Jump && !Fall;

    //--Xử lý cây
    while(true)
    {
       if (count == 0)
       {
        t[nt++] = 95;
        count = rand() % (65 - 35 + 1) + 35; // [35,65]
       }
     count--;

     di_chuyen_xrong(t , nt);
     di_chuyen_mat_dat();
     
     if (_kbhit())
        {
            int key = _getch();
            if (key == 224) key = _getch();//git CMD se doc 2 lan de ra 72 hoac 80
            if ((key == ' '||key == 72 ||key == 'w')&& !Jump &&  !Fall) //Ktra dk khi no chua nhay và chua roi
            {
                Jump = true;
                jumpY = 0;
            }
            else if (key == 80 || key == 's') //cui xuong {CHUA LAM DC}
              {  Duck = true;}
            
             else 
            {
            
                Duck = false;
            }
        }
        //--- Xóa hình cũ trước khi cập nhật vị trí ---
        /*for (int i = 0; i < h; i++) 
        {
        gotoXY(x, y + i);
        cout << space;
        }*/

   
     clear_dino(xdino, ydino - jumpY);//tru di vi tri cu
     if (Jump)
        {
            jumpY++;
            if (jumpY >= jump_height)
            
            {
                Jump = false;
                Fall = true;
            }
        }
     else if (Fall)
        {
            jumpY--;
            if (jumpY <= 0 )
            {
                Fall = false;
                jumpY = 0;
            }
        }
    //--Trạng thái nhảy của khủng long
     if (!Jump && !Duck) //không nhảy không cúi
    { 
        y--;
        if (y < 1) y = 1; // tránh vượt nóc
    } 
    else if (Fall) // rơi
    { 
        y++;
    }
     // --- kiểm tra trạng thái ---
    if (y == ydino - Jump) {
        check = 0; // rơi
    } else if (y >= ydino) {
        y = ydino;
        check = 2; // đứng im
        kt_jump = false;
    }
        draw_dino(xdino, ydino - jumpY);

        di_chuyen_mat_dat();
        draw_mat_dat();
     //--Kết thúc game
     if (game_over (x , y , t , nt) == true)
     {
        break;
     }
     //--- speed---
     Sleep(50);
        
    }
}
    
    
  /* 
    
    //while (true)
   // {
        
        // --- di chuyển khủng long ---
    
    
    // --- vẽ lại khủng long mới ---
    draw_dino(x, y);
    
    }

    }
}*/








