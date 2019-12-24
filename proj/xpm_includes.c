#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <lcom/xpm.h>
#include <stdio.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include "xpms/bg.xpm"
#include "xpms/bg1.xpm"
#include "xpms/block.xpm"
#include "xpms/block1.xpm"
#include "xpms/cursor_simple.xpm"
#include "xpms/A.xpm"
#include "xpms/a.xpm"
#include "xpms/B.xpm"
#include "xpms/b.xpm"
#include "xpms/C.xpm"
#include "xpms/c.xpm"
#include "xpms/D.xpm"
#include "xpms/d.xpm"
#include "xpms/E.xpm"
#include "xpms/e.xpm"
#include "xpms/F.xpm"
#include "xpms/f.xpm"
#include "xpms/G.xpm"
#include "xpms/g.xpm"
#include "xpms/H.xpm"
#include "xpms/h.xpm"
#include "xpms/I.xpm"
#include "xpms/i.xpm"
#include "xpms/J.xpm"
#include "xpms/j.xpm"
#include "xpms/K.xpm"
#include "xpms/k.xpm"
#include "xpms/L.xpm"
#include "xpms/l.xpm"
#include "xpms/M.xpm"
#include "xpms/m.xpm"
#include "xpms/N.xpm"
#include "xpms/n.xpm"
#include "xpms/O.xpm"
#include "xpms/o.xpm"
#include "xpms/P.xpm"
#include "xpms/p.xpm"
#include "xpms/Q.xpm"
#include "xpms/q.xpm"
#include "xpms/R.xpm"
#include "xpms/r.xpm"
#include "xpms/S.xpm"
#include "xpms/s.xpm"
#include "xpms/T.xpm"
#include "xpms/t.xpm"
#include "xpms/U.xpm"
#include "xpms/u.xpm"
#include "xpms/V.xpm"
#include "xpms/v.xpm"
#include "xpms/W.xpm"
#include "xpms/w.xpm"
#include "xpms/X.xpm"
#include "xpms/x.xpm"
#include "xpms/Y.xpm"
#include "xpms/y.xpm"
#include "xpms/Z.xpm"
#include "xpms/z.xpm"
#include "xpms/0.xpm"
#include "xpms/1.xpm"
#include "xpms/2.xpm"
#include "xpms/3.xpm"
#include "xpms/4.xpm"
#include "xpms/5.xpm"
#include "xpms/6.xpm"
#include "xpms/7.xpm"
#include "xpms/8.xpm"
#include "xpms/9.xpm"
#include "xpms/and.xpm"
#include "xpms/apostrophe.xpm"
#include "xpms/comma.xpm"
#include "xpms/dpoint.xpm"
#include "xpms/exclamation.xpm"
#include "xpms/hifen.xpm"
#include "xpms/left.xpm"
#include "xpms/right.xpm"
#include "xpms/point.xpm"
#include "xpms/question.xpm"
#include "xpms/quote.xpm"
#include "xpms/rb1.xpm"
#include "xpms/rb2.xpm"
#include "xpms/rb3.xpm"
#include "xpms/rb4.xpm"
#include "xpms/rb5.xpm"
#include "xpms/rb6.xpm"
#include "xpms/rb7.xpm"
#include "xpms/rb8.xpm"
#include "xpms/rb9.xpm"
#include "xpms/rb10.xpm"
#include "xpms/rb11.xpm"
#include "xpms/rb12.xpm"
#include "xpms/bb1.xpm"
#include "xpms/bb2.xpm"
#include "xpms/bb3.xpm"
#include "xpms/bb4.xpm"
#include "xpms/bb5.xpm"
#include "xpms/bb6.xpm"
#include "xpms/bb7.xpm"
#include "xpms/bb8.xpm"
#include "xpms/bb9.xpm"
#include "xpms/bb10.xpm"
#include "xpms/bb11.xpm"
#include "xpms/bb12.xpm"


static xpm_image_t background;
static xpm_image_t background1;
static xpm_image_t block;
static xpm_image_t block1;
static xpm_image_t mouse_simple;
static xpm_image_t alphabet[73];
static xpm_image_t red_ball_animation[12];
static xpm_image_t blue_ball_animation[12];

void load_pixmaps() {
    xpm_load(background_xpm, XPM_8_8_8, &background);
    xpm_load(background1_xpm, XPM_8_8_8, &background1);
    xpm_load(block_xpm, XPM_8_8_8, &block);
    xpm_load(block1_xpm, XPM_8_8_8, &block1);
    xpm_load(cursor_simple_xpm, XPM_8_8_8, &mouse_simple);

    xpm_load(A_xpm, XPM_8_8_8, &alphabet[0]);
    xpm_load(B_xpm, XPM_8_8_8, &alphabet[1]);
    xpm_load(C_xpm, XPM_8_8_8, &alphabet[2]);
    xpm_load(D_xpm, XPM_8_8_8, &alphabet[3]);
    xpm_load(E_xpm, XPM_8_8_8, &alphabet[4]);
    xpm_load(F_xpm, XPM_8_8_8, &alphabet[5]);
    xpm_load(G_xpm, XPM_8_8_8, &alphabet[6]);
    xpm_load(H_xpm, XPM_8_8_8, &alphabet[7]);
    xpm_load(I_xpm, XPM_8_8_8, &alphabet[8]);
    xpm_load(J_xpm, XPM_8_8_8, &alphabet[9]);
    xpm_load(K_xpm, XPM_8_8_8, &alphabet[10]);
    xpm_load(L_xpm, XPM_8_8_8, &alphabet[11]);
    xpm_load(M_xpm, XPM_8_8_8, &alphabet[12]);
    xpm_load(N_xpm, XPM_8_8_8, &alphabet[13]);
    xpm_load(O_xpm, XPM_8_8_8, &alphabet[14]);
    xpm_load(P_xpm, XPM_8_8_8, &alphabet[15]);
    xpm_load(Q_xpm, XPM_8_8_8, &alphabet[16]);
    xpm_load(R_xpm, XPM_8_8_8, &alphabet[17]);
    xpm_load(S_xpm, XPM_8_8_8, &alphabet[18]);
    xpm_load(T_xpm, XPM_8_8_8, &alphabet[19]);
    xpm_load(U_xpm, XPM_8_8_8, &alphabet[20]);
    xpm_load(V_xpm, XPM_8_8_8, &alphabet[21]);
    xpm_load(W_xpm, XPM_8_8_8, &alphabet[22]);
    xpm_load(X_xpm, XPM_8_8_8, &alphabet[23]);
    xpm_load(Y_xpm, XPM_8_8_8, &alphabet[24]);
    xpm_load(Z_xpm, XPM_8_8_8, &alphabet[25]);
    xpm_load(a_xpm, XPM_8_8_8, &alphabet[26]);
    xpm_load(b_xpm, XPM_8_8_8, &alphabet[27]);
    xpm_load(c_xpm, XPM_8_8_8, &alphabet[28]);
    xpm_load(d_xpm, XPM_8_8_8, &alphabet[29]);
    xpm_load(e_xpm, XPM_8_8_8, &alphabet[30]);
    xpm_load(f_xpm, XPM_8_8_8, &alphabet[31]);
    xpm_load(g_xpm, XPM_8_8_8, &alphabet[32]);
    xpm_load(h_xpm, XPM_8_8_8, &alphabet[33]);
    xpm_load(i_xpm, XPM_8_8_8, &alphabet[34]);
    xpm_load(j_xpm, XPM_8_8_8, &alphabet[35]);
    xpm_load(k_xpm, XPM_8_8_8, &alphabet[36]);
    xpm_load(l_xpm, XPM_8_8_8, &alphabet[37]);
    xpm_load(m_xpm, XPM_8_8_8, &alphabet[38]);
    xpm_load(n_xpm, XPM_8_8_8, &alphabet[39]);
    xpm_load(o_xpm, XPM_8_8_8, &alphabet[40]);
    xpm_load(p_xpm, XPM_8_8_8, &alphabet[41]);
    xpm_load(q_xpm, XPM_8_8_8, &alphabet[42]);
    xpm_load(r_xpm, XPM_8_8_8, &alphabet[43]);
    xpm_load(s_xpm, XPM_8_8_8, &alphabet[44]);
    xpm_load(t_xpm, XPM_8_8_8, &alphabet[45]);
    xpm_load(u_xpm, XPM_8_8_8, &alphabet[46]);
    xpm_load(v_xpm, XPM_8_8_8, &alphabet[47]);
    xpm_load(w_xpm, XPM_8_8_8, &alphabet[48]);
    xpm_load(x_xpm, XPM_8_8_8, &alphabet[49]);
    xpm_load(y_xpm, XPM_8_8_8, &alphabet[50]);
    xpm_load(z_xpm, XPM_8_8_8, &alphabet[51]);
    xpm_load(_0_xpm, XPM_8_8_8, &alphabet[52]);
    xpm_load(_1_xpm, XPM_8_8_8, &alphabet[53]);
    xpm_load(_2_xpm, XPM_8_8_8, &alphabet[54]);
    xpm_load(_3_xpm, XPM_8_8_8, &alphabet[55]);
    xpm_load(_4_xpm, XPM_8_8_8, &alphabet[56]);
    xpm_load(_5_xpm, XPM_8_8_8, &alphabet[57]);
    xpm_load(_6_xpm, XPM_8_8_8, &alphabet[58]);
    xpm_load(_7_xpm, XPM_8_8_8, &alphabet[59]);
    xpm_load(_8_xpm, XPM_8_8_8, &alphabet[60]);
    xpm_load(_9_xpm, XPM_8_8_8, &alphabet[61]);
    xpm_load(point_xpm, XPM_8_8_8, &alphabet[62]);
    xpm_load(comma_xpm, XPM_8_8_8, &alphabet[63]);
    xpm_load(dpoint_xpm, XPM_8_8_8, &alphabet[64]);
    xpm_load(question_xpm, XPM_8_8_8, &alphabet[65]);
    xpm_load(exclamation_xpm, XPM_8_8_8, &alphabet[66]);
    xpm_load(hifen_xpm, XPM_8_8_8, &alphabet[67]);
    xpm_load(quote_xpm, XPM_8_8_8, &alphabet[68]);
    xpm_load(apostrophe_xpm, XPM_8_8_8, &alphabet[69]);
    xpm_load(and_xpm, XPM_8_8_8, &alphabet[70]);
    xpm_load(left_xpm, XPM_8_8_8, &alphabet[71]);
    xpm_load(right_xpm, XPM_8_8_8, &alphabet[72]);

    xpm_load(rb1_xpm, XPM_8_8_8, &red_ball_animation[0]);
    xpm_load(rb2_xpm, XPM_8_8_8, &red_ball_animation[1]);
    xpm_load(rb3_xpm, XPM_8_8_8, &red_ball_animation[2]);
    xpm_load(rb4_xpm, XPM_8_8_8, &red_ball_animation[3]);
    xpm_load(rb5_xpm, XPM_8_8_8, &red_ball_animation[4]);
    xpm_load(rb6_xpm, XPM_8_8_8, &red_ball_animation[5]);
    xpm_load(rb7_xpm, XPM_8_8_8, &red_ball_animation[6]);
    xpm_load(rb8_xpm, XPM_8_8_8, &red_ball_animation[7]);
    xpm_load(rb9_xpm, XPM_8_8_8, &red_ball_animation[8]);
    xpm_load(rb10_xpm, XPM_8_8_8, &red_ball_animation[9]);
    xpm_load(rb11_xpm, XPM_8_8_8, &red_ball_animation[10]);
    xpm_load(rb12_xpm, XPM_8_8_8, &red_ball_animation[11]);

    xpm_load(bb1_xpm, XPM_8_8_8, &blue_ball_animation[0]);
    xpm_load(bb2_xpm, XPM_8_8_8, &blue_ball_animation[1]);
    xpm_load(bb3_xpm, XPM_8_8_8, &blue_ball_animation[2]);
    xpm_load(bb4_xpm, XPM_8_8_8, &blue_ball_animation[3]);
    xpm_load(bb5_xpm, XPM_8_8_8, &blue_ball_animation[4]);
    xpm_load(bb6_xpm, XPM_8_8_8, &blue_ball_animation[5]);
    xpm_load(bb7_xpm, XPM_8_8_8, &blue_ball_animation[6]);
    xpm_load(bb8_xpm, XPM_8_8_8, &blue_ball_animation[7]);
    xpm_load(bb9_xpm, XPM_8_8_8, &blue_ball_animation[8]);
    xpm_load(bb10_xpm, XPM_8_8_8, &blue_ball_animation[9]);
    xpm_load(bb11_xpm, XPM_8_8_8, &blue_ball_animation[10]);
    xpm_load(bb12_xpm, XPM_8_8_8, &blue_ball_animation[11]);
}

xpm_image_t get_background() { 
    return background; 
}

xpm_image_t get_background1(){
    return background1;
}

xpm_image_t get_block() { return block; }

xpm_image_t get_block1() { return block1; }

xpm_image_t get_mouse_simple() { return mouse_simple; }

xpm_image_t get_letter(char c){
    if (c >= 65 && c <= 90) {
        return alphabet[c%65];
    } else if (c >= 97 && c <= 122) {
        return alphabet[c%97 + 26];
    } else if (c >= 48 && c <= 57) {
        return alphabet[c%48 + 52];
    } else if (c == 46) return alphabet[62];
    else if (c == 44) return alphabet[63];
    else if (c == 58) return alphabet[64];
    else if (c == 63) return alphabet[65];
    else if (c == 33) return alphabet[66];
    else if (c == 45) return alphabet[67];
    else if (c == 34) return alphabet[68];
    else if (c == 39) return alphabet[69];
    else if (c == 38) return alphabet[70];
    else if (c == 40) return alphabet[71];
    else if (c == 41) return alphabet[72];
    else return alphabet[0];
}

xpm_image_t * get_red_ball_animation() { return red_ball_animation; }

xpm_image_t * get_blue_ball_animation() { return blue_ball_animation; }
