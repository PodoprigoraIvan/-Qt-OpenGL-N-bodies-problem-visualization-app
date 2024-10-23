#include "addnewspherewidget.h"

Vector3DInput::Vector3DInput(QString labelText) :
    nameLabel(labelText),
    xLabel("X"), yLabel("Y"), zLabel("Z")
    {
    setLayout(&layout);
    layout.addWidget(&nameLabel);
    layout.addWidget(&xLabel);
    layout.addWidget(&xInput);
    layout.addWidget(&yLabel);
    layout.addWidget(&yInput);
    layout.addWidget(&zLabel);
    layout.addWidget(&zInput);
    xLabel.setAlignment(Qt::AlignRight);
    yLabel.setAlignment(Qt::AlignRight);
    zLabel.setAlignment(Qt::AlignRight);
    xInput.setRange(-1000.0, 1000.0); yInput.setRange(-1000.0, 1000.0); zInput.setRange(-1000.0, 1000.0);
    setMinimumHeight(30);
    setMaximumHeight(35);
    setStyleSheet("background-color: lightblue;");
}

QVector3D Vector3DInput::getVector(){
    return QVector3D(xInput.value(), yInput.value(), zInput.value());
}

AddNewSphereWidget::AddNewSphereWidget(PhysycsModel* physycsModdel, DrawWidget* drawer, QWidget *parent)
    : QWidget(parent),
    physycsModel(physycsModdel),
    title("New body adding"), velocityLengthLabel("Velocity length"), radiusLabel("Radius"), massLabel("Mass"), spawnOnLable("Spawn on"), velocityDirectionLabel("Velocity direction"), errorLabel(""), colorLabel(" "),
    cameraPosBtn("Camera position"), specificPosBtn("Scpecific position"),
    cameraDirBtn("Camera direction"), specificDirBtn("Specific direction"),
    addButton("Add"), pickColorButton("Choose color"),
    posVecInput("Position"), dirVecInput("Direction"),
    color(Qt::blue),
    drawer(drawer)
{
    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    setSizePolicy(policy);

    setLayout(&layout);
    layout.setContentsMargins(5, 5, 5, 5);
    layout.addWidget(&title, 0, 0, 1, 2);
    layout.addWidget(&velocityLengthLabel, 1, 0);
    layout.addWidget(&velocityLengthInput, 1, 1);
    velocityLengthInput.setRange(0.0, 1000.0);

    layout.addWidget(&radiusLabel, 2, 0);
    layout.addWidget(&radiusInput, 2, 1);
    radiusInput.setRange(0.1, 100.0);
    radiusInput.setValue(1.0);

    layout.addWidget(&massLabel, 3, 0);
    layout.addWidget(&massInput, 3, 1);
    massInput.setRange(0.1, 1000000.0);

    spawnSelectGroup.addButton(&cameraPosBtn);
    QObject::connect(&cameraPosBtn, &QRadioButton::toggled, this, &AddNewSphereWidget::onCameraPosToggle);
    cameraPosBtn.setChecked(true);

    spawnSelectGroup.addButton(&specificPosBtn);
    QObject::connect(&specificPosBtn, &QRadioButton::toggled, this, &AddNewSphereWidget::onSpecificPosToggle);

    layout.addWidget(&spawnOnLable, 4, 0, 2, 1);
    layout.addWidget(&cameraPosBtn, 4, 1);
    layout.addWidget(&specificPosBtn, 5, 1);

    layout.addWidget(&posVecInput, 6, 0, 1, 2);
    posVecInput.setDisabled(true);

    velocityTypeSelectGroup.addButton(&cameraDirBtn);
    QObject::connect(&cameraDirBtn, &QRadioButton::toggled, this, &AddNewSphereWidget::onCameraDirToggle);
    cameraDirBtn.setChecked(true);

    velocityTypeSelectGroup.addButton(&specificDirBtn);
    QObject::connect(&specificDirBtn, &QRadioButton::toggled, this, &AddNewSphereWidget::onSpecificDirToggle);

    layout.addWidget(&velocityDirectionLabel, 7, 0, 2, 1);
    layout.addWidget(&cameraDirBtn, 7, 1);
    layout.addWidget(&specificDirBtn, 8, 1);

    layout.addWidget(&dirVecInput, 9, 0, 1, 2);
    dirVecInput.setDisabled(true);

    layout.addWidget(&pickColorButton, 10, 0);
    connect(&pickColorButton, &QPushButton::clicked, this, &AddNewSphereWidget::openColorDialog);

    layout.addWidget(&colorLabel, 10, 1);
    colorLabel.setStyleSheet("background-color: " + color.name() + "; color: " + color.name() + "; border-radius: 10");

    layout.addWidget(&addButton, 11, 0, 1, 2);
    connect(&addButton, &QPushButton::clicked, this, &AddNewSphereWidget::addSphereButtonClick);

    layout.addWidget(&errorLabel, 12, 0, 1, 2);
    QSize sz = sizeHint();
    setMinimumHeight(sz.height());
    setMaximumHeight(sz.height());
    setMaximumWidth(300);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: #cccccc;");
}

void AddNewSphereWidget::setDrawer(DrawWidget * newDrawer){
    drawer = newDrawer;
}

void AddNewSphereWidget::openColorDialog() {
    QColor chosenColor = QColorDialog::getColor(color, this, "Choose new body color");
    if (chosenColor.isValid()) {
        color = chosenColor;
        colorLabel.setStyleSheet("background-color: " + color.name() + "; color: " + color.name() + "; border-radius: 10");
    }
}

void AddNewSphereWidget::addSphereButtonClick(){
    if (drawer == nullptr)
        return;
    QVector3D pos;
    if (cameraPosBtn.isChecked()) pos = drawer->getCameraPos();
    else pos = posVecInput.getVector();

    float velocityLength = velocityLengthInput.value();
    QVector3D velocityDir;
    if (cameraDirBtn.isChecked()) velocityDir = drawer->getCameraDirection();
    else velocityDir = dirVecInput.getVector();
    velocityDir.normalize();
    QVector3D velocityVector = velocityLength * velocityDir;

    float mass = massInput.value();

    float radius = radiusInput.value();

    float r, g, b;
    color.getRgbF(&r, &g, &b);
    QVector3D rgbVecColor(r, g, b);
    bool canAdd = physycsModel->canAddSphere(pos, radius);
    if (canAdd){
        SphereInfo newSphere(pos, rgbVecColor, mass, radius, 300, velocityVector);
        physycsModel->addSphere(newSphere);
        emit addSphere(newSphere.getID(), color);
        errorLabel.setText("");
    } else {
        errorLabel.setText("Can't add: intersection");
        errorLabel.setStyleSheet("color: red;");
    }
}

void AddNewSphereWidget::addSphereManually(QColor color, SphereInfo sphere){
    bool canAdd = physycsModel->canAddSphere(sphere.getPos(), sphere.r);
    if (canAdd){
        this->color = color;
        physycsModel->addSphere(sphere);
        emit addSphere(sphere.getID(), color);
    }
}

void AddNewSphereWidget::onCameraPosToggle(bool active){
    if (active)
        posVecInput.setDisabled(true);
}

void AddNewSphereWidget::onSpecificPosToggle(bool active){
    if (active)
        posVecInput.setDisabled(false);
}

void AddNewSphereWidget::onCameraDirToggle(bool active){
    if (active)
        dirVecInput.setDisabled(true);
}

void AddNewSphereWidget::onSpecificDirToggle(bool active){
    if (active)
        dirVecInput.setDisabled(false);
}
