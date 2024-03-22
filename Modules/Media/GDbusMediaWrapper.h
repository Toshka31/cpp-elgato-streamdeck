#ifndef ELGATO_STREAMDECK_GDBUSMEDIAWRAPPER_H
#define ELGATO_STREAMDECK_GDBUSMEDIAWRAPPER_H

#include <glib.h>
#include <gio/gio.h>

#include <vector>
#include <string>
#include <any>

class GDbusMediaWrapper {
public:
    static void playPause();
    static void next();
    static void previous();
    static bool hasNext();
    static bool hasPrevious();
    static bool isPlaying();
private:
    static std::vector<std::string> getMediaPlayers();
    static bool sendMediaPlayerCommand(const std::string &dbus_player, const std::string &command);
    static std::any getMediaPlayerProperty(const std::string &dbus_player, const std::string &property);
};

#endif //ELGATO_STREAMDECK_GDBUSMEDIAWRAPPER_H
