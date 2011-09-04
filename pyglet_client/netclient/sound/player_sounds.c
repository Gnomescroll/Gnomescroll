#include "player_sounds.h"

void load_player_gunshot() {
    player_gunshot = load_2d_sound("./media/sound/wav/semishoot.wav");
}

void play_player_gunshot() {
    player_gunshot_channel = play_2d_sound(player_gunshot);
}

void end_player_gunshot() {
    release_sound(player_gunshot);
}

