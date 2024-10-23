#include "sphereslistwidget.h"

SphereListItem::SphereListItem(int ID, QColor sphereColor)
    : ID(ID)
    , sphereColor(sphereColor), traсkingColor("#a45ba1"), defaultColor("#cccccc")
    , idLabel(QString("ID: %1").arg(ID))
    , sphereColorLabel("  ")
    , trackBtn("Track")
    , removeBtn("Remove") {
    setLayout(&layout);
    layout.addWidget(&idLabel);
    layout.addWidget(&sphereColorLabel);
    layout.addWidget(&trackBtn);
    layout.addWidget(&removeBtn);

    QSizePolicy policy = sizePolicy();
    policy.setVerticalPolicy(QSizePolicy::Maximum);
    setSizePolicy(policy);

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color:" + defaultColor.name() + ";");
    sphereColorLabel.setStyleSheet("background-color: " + sphereColor.name() + "; border-radius: 10");
    QObject::connect(&trackBtn, &QPushButton::clicked, this, &SphereListItem::onTrackBtnPressed);
    QObject::connect(&removeBtn, &QPushButton::clicked, this, &SphereListItem::onRemoveBtnPressed);
}

int SphereListItem::getID() {
    return ID;
}

void SphereListItem::useTrackingBackgroundColor(bool use){
    if (use)
        setStyleSheet("background-color:" + traсkingColor.name() + "; color: #dbf1e5");
    else
        setStyleSheet("background-color:" + defaultColor.name() + ";");
}

void SphereListItem::onTrackBtnPressed(){
    emit trackSphere(ID);
}

void SphereListItem::onRemoveBtnPressed() {
    emit removeSphere(ID);
}



SpheresListWidget::SpheresListWidget(PhysycsModel* physycsModel, QWidget *parent)
    : QWidget(parent)
    , physycsModel(physycsModel)
    , stopTrackBtn("Stop tracking")
{
    setLayout(&layout);
    layout.addWidget(&stopTrackBtn);
    scrollArea.setWidgetResizable(true);
    container.setLayout(&scrollLayout);
    scrollArea.setWidget(&container);
    layout.addWidget(&scrollArea);
    scrollLayout.setAlignment(Qt::AlignTop);
    QObject::connect(&stopTrackBtn, &QPushButton::clicked, this, &SpheresListWidget::stopTrackBtnPressed);

    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    setSizePolicy(policy);
    setMaximumWidth(300);
}

void SpheresListWidget::onAdd(int ID, QColor color) {
    widgets.push_back(new SphereListItem(ID, color));
    scrollLayout.addWidget(widgets[widgets.size()-1]);
    QObject::connect(widgets[widgets.size()-1], &SphereListItem::trackSphere, this, &SpheresListWidget::onTrack);
    QObject::connect(widgets[widgets.size()-1], &SphereListItem::removeSphere, this, &SpheresListWidget::onRemove);
}

void SpheresListWidget::onTrack(int ID){
    for(int i = 0; i < widgets.size(); i++){
        if (widgets[i]->getID() == ID){
            widgets[i]->useTrackingBackgroundColor(true);
            emit trackSphere(ID);
        } else
            widgets[i]->useTrackingBackgroundColor(false);
    }
}

void SpheresListWidget::onRemove(int ID){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Remove confirmation", QString("Remove body with ID = %1?").arg(ID), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        for(int i = 0; i < widgets.size(); i++){
            if (widgets[i]->getID() == ID){
                scrollLayout.removeWidget(widgets[i]);
                widgets[i]->deleteLater();
                widgets.erase(widgets.begin()+i);
                physycsModel->removeSphereOnID(ID);
                emit removeOnID(ID);
                return;
            }
        }
    }
}

void SpheresListWidget::stopTrackBtnPressed(){
    for(int i = 0; i < widgets.size(); i++)
        widgets[i]->useTrackingBackgroundColor(false);
    emit stopTracking();
}
