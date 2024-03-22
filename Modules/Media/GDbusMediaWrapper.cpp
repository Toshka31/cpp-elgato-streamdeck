#include "GDbusMediaWrapper.h"

#include <iostream>

void GDbusMediaWrapper::playPause() {
    auto vec_players = getMediaPlayers();
    for (const auto &player : vec_players) {
        sendMediaPlayerCommand(player, "PlayPause");
    }
}

void GDbusMediaWrapper::next() {
    auto vec_players = getMediaPlayers();
    for (const auto &player : vec_players) {
        sendMediaPlayerCommand(player, "Next");
    }
}

void GDbusMediaWrapper::previous() {
    auto vec_players = getMediaPlayers();
    for (const auto &player : vec_players) {
        sendMediaPlayerCommand(player, "Previous");
    }
}

bool GDbusMediaWrapper::hasNext() {
    auto vec_players = getMediaPlayers();
    for (const auto &player : vec_players) {
        std::any ret = getMediaPlayerProperty(player, "CanGoNext");
        return std::any_cast<bool>(ret);
    }
    return false;
}

bool GDbusMediaWrapper::hasPrevious() {
    auto vec_players = getMediaPlayers();
    for (const auto &player : vec_players) {
        std::any ret = getMediaPlayerProperty(player, "CanGoPrevious");
        return std::any_cast<bool>(ret);
    }
    return false;
}

bool GDbusMediaWrapper::isPlaying() {
    auto vec_players = getMediaPlayers();
    for (const auto &player : vec_players) {
        std::any ret = getMediaPlayerProperty(player, "PlaybackStatus");
        if (any_cast<std::string>(ret) == "Playing")
            return true;
    }
    return false;
}

std::vector<std::string> GDbusMediaWrapper::getMediaPlayers() {
    std::vector<std::string> result;
    GDBusConnection *session_bus = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, nullptr);
    if (session_bus) {
        GDBusMessage *msg = g_dbus_message_new_method_call("org.freedesktop.DBus",
                                                           "/org/freedesktop/DBus",
                                                           "org.freedesktop.DBus",
                                                           "ListNames"
        );

        GDBusMessage *reply = g_dbus_connection_send_message_with_reply_sync(session_bus, msg,
                                                                             G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                                             30000, // NOLINT: flatpak legacy code
                                                                             nullptr,
                                                                             nullptr,
                                                                             nullptr
        );
        if (reply) {
            auto type = g_dbus_message_get_message_type(reply);
            if (type == G_DBUS_MESSAGE_TYPE_ERROR) {
                g_autoptr(GError) local_error = nullptr;
                g_dbus_message_to_gerror(reply, &local_error);
                std::cout << "portal: reply received with error: " << local_error->message << "\n";
            } else if (type == G_DBUS_MESSAGE_TYPE_METHOD_RETURN) {
                GVariant *v = g_dbus_message_get_body(reply);
                GVariantIter *iter;
                gchar *str;

                g_variant_get(v, "(as)", &iter);
                while (g_variant_iter_loop(iter, "s", &str)) {
                    if (std::string(str).starts_with("org.mpris.MediaPlayer2")) {
                        result.emplace_back(str);
                    }
                }
                g_variant_iter_free(iter);
            }
        }
        g_object_unref(msg);
        g_object_unref(reply);
    }

    return result;
}

bool GDbusMediaWrapper::sendMediaPlayerCommand(const std::string &dbus_player, const std::string &command) {
    bool result = false;
    GDBusConnection *session_bus = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, nullptr);
    if (session_bus) {
        g_autoptr(GDBusMessage) msg_command = g_dbus_message_new_method_call(dbus_player.c_str(),
                                                                             "/org/mpris/MediaPlayer2",
                                                                             "org.mpris.MediaPlayer2.Player",
                                                                             command.c_str());  // PlayPause | Next | Previous

        result = g_dbus_connection_send_message(session_bus,
                                                msg_command,
                                                G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                nullptr,
                                                nullptr
        );

        if (result)
            result = g_dbus_connection_flush_sync(session_bus, nullptr, nullptr);
    }
    return result;
}

std::any GDbusMediaWrapper::getMediaPlayerProperty(const std::string &dbus_player, const std::string &property) {
    std::any ret;
    GDBusConnection *session_bus = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, nullptr);
    if (session_bus) {
        GDBusMessage *msg_prop = g_dbus_message_new_method_call(dbus_player.c_str(),
                                                                "/org/mpris/MediaPlayer2",
                                                                "org.freedesktop.DBus.Properties",
                                                                "Get"
        );

        g_dbus_message_set_body(msg_prop, g_variant_new("(ss)", "org.mpris.MediaPlayer2.Player", property.c_str()));

        GDBusMessage *reply = g_dbus_connection_send_message_with_reply_sync(session_bus, msg_prop,
                                                                             G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                                             30000, // NOLINT: flatpak legacy code
                                                                             nullptr,
                                                                             nullptr,
                                                                             nullptr
        );
        if (reply) {
            auto type = g_dbus_message_get_message_type(reply);
            if (type == G_DBUS_MESSAGE_TYPE_ERROR) {
                g_autoptr(GError) local_error = nullptr;
                g_dbus_message_to_gerror(reply, &local_error);
                std::cout << "reply received with error: " << local_error->message << "\n";
            } else if (type == G_DBUS_MESSAGE_TYPE_METHOD_RETURN) {
                // iterate over the list
                GVariant *variant = g_dbus_message_get_body(reply);
                g_autoptr(GVariant) child = nullptr;
                g_variant_get(variant, "(v)", &child);
                GVariantClass var_type = g_variant_classify(child);

                switch (var_type) {
                    case G_VARIANT_CLASS_STRING:
                        gchar *str_out;
                        g_variant_get(child, "s", &str_out);
                        ret = std::make_any<std::string>(str_out);
                        break;
                    case G_VARIANT_CLASS_BOOLEAN:
                        gboolean bool_out;
                        g_variant_get(child, "b", &bool_out);
                        ret = std::make_any<bool>(bool_out);
                        break;
                    default:
                        break;
                }
            }
        }
        g_object_unref(msg_prop);
        g_object_unref(reply);
    }
    return ret;
}