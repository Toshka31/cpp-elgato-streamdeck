#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTreeWidget>

class ManagingWidget : public QWidget
{
public:
    explicit ManagingWidget(std::shared_ptr<StreamDeckClient> client, QWidget *parent)
        : QWidget(parent), m_client(client)
    {
        auto *lt_main = new QHBoxLayout(this);

        lt_main->addWidget(new QLabel("Device", this));
        auto cmb_devices = new QComboBox(this);
        auto vec_devices = m_client->getDevicesList();
        for (const auto& str_device : vec_devices)
            cmb_devices->addItem(QString::fromStdString(str_device));
        lt_main->addWidget(cmb_devices);

        auto current_device = cmb_devices->currentText().toStdString();

        lt_main->addStretch();

        lt_main->addWidget(new QLabel("Profile", this));
        auto cmb_profiles = new QComboBox(this);
        auto vec_profiles = m_client->getDeviceProfiles(current_device);
        auto cur_profile = m_client->getDeviceCurrentProfile(current_device);
        for (const auto& str_profile : vec_profiles)
            cmb_profiles->addItem(QString::fromStdString(str_profile));
        cmb_profiles->setCurrentText(QString::fromStdString(cur_profile));
        lt_main->addWidget(cmb_profiles);

        lt_main->addStretch();

        lt_main->addWidget(new QLabel("Page", this));
        auto cmb_pages = new QComboBox(this);
        auto vec_pages = m_client->getDevicePages(current_device);
        auto cur_page = m_client->getDeviceCurrentPage(current_device);
        for (const auto& str_page : vec_pages)
            cmb_pages->addItem(QString::fromStdString(str_page));
        cmb_pages->setCurrentText(QString::fromStdString(cur_page));
        lt_main->addWidget(cmb_pages);
    }
private:
    std::shared_ptr<StreamDeckClient> m_client;
};

class DeckButtonsWidget : public QWidget
{
public:
    explicit DeckButtonsWidget(QWidget *parent) : QWidget(parent)
    {
        auto *lt_grid = new QGridLayout(this);
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 3; ++j) {
                auto *btn = new QPushButton(this);
                btn->sizePolicy();
                lt_grid->addWidget(new QPushButton(this), j, i);
            }
    }
};

class ModulesListWidget : public QWidget
{
public:
    explicit ModulesListWidget(QWidget *parent) : QWidget(parent)
    {
        auto *lt_main = new QVBoxLayout(this);
        lt_main->addWidget(new QTreeWidget(this));
    }
};

MainWindow::MainWindow(
        std::shared_ptr<StreamDeckClient> client,
        QWidget *parent,
        Qt::WindowFlags flags)
        : QMainWindow(parent, flags),
        m_client(client)
{
    auto *central_widget = new QWidget(this);

    auto *lt_hor = new QHBoxLayout(central_widget);

    auto *lt_vert = new QVBoxLayout(central_widget);
    lt_vert->addWidget(new ManagingWidget(client, central_widget), 1);
    lt_vert->addWidget(new DeckButtonsWidget(central_widget), 3);

    lt_hor->addLayout(lt_vert, 2);
    lt_hor->addWidget(new ModulesListWidget(central_widget), 1);

    central_widget->setLayout(lt_hor);

    setCentralWidget(central_widget);
}
