// http://doc.qt.io/qt-5/qaudiooutput.html#QAudioOutput
// https://stackoverflow.com/questions/21310324/play-notification-frequency-x-sound-in-qt-easiest-way
// https://github.com/jordan9001/SpectrumDraw

#include "patterneditorwidget.h"

#include <QDebug>

#define MAX_PARAMS_LINE 20
#define ELEMENT_SIZE 20

#define LABELS_NUM   6
#define COMMANDS_NUM 6
#define NOTES_NUM    60

#define FILE_EXTENSION ".gbs"
#define T_SAVE_FILE "Save"
#define T_LOAD_FILE "Load"
#define T_GENERATE  "Generate"
#define T_PLAY_PATTERN "Play"

#define T_FILE_MARK "GBS"

#define T_PATTERN_START "const uint16_t "
#define T_PATTERN_PROGMEM "[] PROGMEM = {\n"
#define T_PATTERN_END "0x0000\n};"

// --------------------------------- //
#define SAMPLE_RATE 8000
#define CH_NUMBER   1
#define BIT_DEPTH   8
#define CODEC_TYPE  "audio/pcm"

#define FRAME_LONG  50
#define NOTE_A4_POS 23
#define FREQ_CONST  ((2.0 * M_PI) / SAMPLE_RATE)
#define MAX_VOL     60
// --------------------------------- //


int currentParamsLine = 0;
int maxParamLines = MAX_PARAMS_LINE;

QString columnLabels[] = {
    "Note/Cmd", "Note",
    "Duration", "Command",
    "X", "Y"
};

QString commandNames[] = {
    "0 Note Volume", "1 Instrument",
    "2 Volume Slide", "3 Arpeggio",
    "4 Tremolo", "None"
};

QString noteNames[] = {
    "A#2", "B2", "C3", "C#3", "D3",
    "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3", "C4",
    "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4",
    "B4", "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5",
    "A5", "A#5", "B5", "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6",
    "G6", "G#6", "A6", "A#6", "B6", "C7", "C#7", "D7", "D#7", "E7",
    "F7", "F#7", "G7", "G#7", "---"
};

PatternEditorWidget::PatternEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupAudioGenerator();

    gInputLayout = new QGridLayout();

    makeTableEditor();

    teResultPattern = new QTextEdit();
    teResultPattern->setMaximumHeight((this->width() / ELEMENT_SIZE*2));
    gInputLayout->addWidget(teResultPattern, currentParamsLine++, 0, 1, 0);

    QPushButton *pbSave = new QPushButton(T_SAVE_FILE);
    gInputLayout->addWidget(pbSave, currentParamsLine, 0);
    connect(pbSave, SIGNAL(pressed()), this, SLOT(saveFilePattern()));

    QPushButton *pbLoad = new QPushButton(T_LOAD_FILE);
    gInputLayout->addWidget(pbLoad, currentParamsLine, 1);
    connect(pbLoad, SIGNAL(pressed()), this, SLOT(loadFilePattern()));

    QPushButton *pbPlay = new QPushButton(T_PLAY_PATTERN);
    gInputLayout->addWidget(pbPlay, currentParamsLine, 4);
    connect(pbPlay, SIGNAL(pressed()), this, SLOT(playPatterns()));

    QPushButton *pbGenerate = new QPushButton(T_GENERATE);
    gInputLayout->addWidget(pbGenerate, currentParamsLine, 5);
    connect(pbGenerate, SIGNAL(pressed()), this, SLOT(generatePattern()));

    setLayout(gInputLayout);
}

PatternEditorWidget::~PatternEditorWidget()
{
    deleteParamLines();

    delete gInputLayout;
    delete teResultPattern;
    delete tableEditor;
}

void PatternEditorWidget::deleteParamLines(void)
{
    for(int i=0; i < maxParamLines; i++) {
        delete noteInstrumentList.takeFirst();
        delete notesList.takeFirst();
        delete commandTypeList.takeFirst();
        delete commandXList.takeFirst();
        delete commandYList.takeFirst();
    }
}

void PatternEditorWidget::addFieldDescription(void)
{
    /*
    for(int i=0; i < LABELS_NUM; i++) {
        tableTest->setHorizontalHeaderItem(i, new QTableWidgetItem(columnLabels[i]));
    }
    */
    QStringList labelsList;
    labelsList << "Note/Cmd" << "Note" << "Duration" << "Command" << "X" << "Y";
    tableEditor->setHorizontalHeaderLabels(labelsList);
}

void PatternEditorWidget::makeTableEditor(void)
{
    tableEditor = new QTableWidget(maxParamLines, LABELS_NUM);
    //tableEditor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    addFieldDescription();

    for(int i=0; i < maxParamLines; i++) {
        int hLineCount =0;
        addNewLine();
        tableEditor->setCellWidget(i, hLineCount++, noteInstrumentList.at(i));
        tableEditor->setCellWidget(i, hLineCount++, notesList.at(i));
        tableEditor->setCellWidget(i, hLineCount++, noteDurationList.at(i));
        tableEditor->setCellWidget(i, hLineCount++, commandTypeList.at(i));
        tableEditor->setCellWidget(i, hLineCount++, commandXList.at(i));
        tableEditor->setCellWidget(i, hLineCount++, commandYList.at(i));
    }

    tableEditor->resizeColumnsToContents();
    gInputLayout->addWidget(tableEditor, currentParamsLine++, 0, 1, 0);
}

void PatternEditorWidget::addNewLine(void)
{
    QCheckBox *cbNoteInstrumentType = new QCheckBox();
    noteInstrumentList.append(cbNoteInstrumentType);
    connect(cbNoteInstrumentType, SIGNAL(stateChanged(int)), this, SLOT(checkBoxChanged()));

    QComboBox *cbNoteType = new QComboBox();
    for(int i=0; i< NOTES_NUM; i++) {
        cbNoteType->addItem(noteNames[i]);
    }
    cbNoteType->setCurrentIndex(NOTES_NUM-1);
    cbNoteType->setView(new QListView);
    notesList.append(cbNoteType);

    QLineEdit *leNoteDurationFrames = new QLineEdit();
    leNoteDurationFrames->setMinimumWidth((this->width() / ELEMENT_SIZE));
    leNoteDurationFrames->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    leNoteDurationFrames->setPlaceholderText("Frames");
    leNoteDurationFrames->setMaxLength(3);
    noteDurationList.append(leNoteDurationFrames);

    QComboBox *cbCommandType = new QComboBox();
    for(int i=0; i< COMMANDS_NUM; i++) {
        cbCommandType->addItem(commandNames[i]);
    }
    cbCommandType->setCurrentIndex(COMMANDS_NUM-1);
    cbCommandType->setDisabled(true);
    cbCommandType->setView(new QListView);
    commandTypeList.append(cbCommandType);

    QLineEdit *leCommandX = new QLineEdit();
    leCommandX->setMinimumWidth((this->width() / ELEMENT_SIZE));
    //leCommandX->setMaximumWidth((this->width() / ELEMENT_SIZE*2));
    leCommandX->setDisabled(true);
    leCommandX->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    leCommandX->setMaxLength(2);
    commandXList.append(leCommandX);

    QLineEdit *leCommandY = new QLineEdit();
    leCommandY->setMinimumWidth((this->width() / ELEMENT_SIZE));
    //leCommandY->setMaximumWidth((this->width() / ELEMENT_SIZE*2));
    leCommandY->setDisabled(true);
    leCommandY->setValidator(new QRegExpValidator(QRegExp("[0-9-]+")));
    leCommandY->setMaxLength(3);
    commandYList.append(leCommandY);
}

void PatternEditorWidget::checkBoxChanged(void)
{
    for(int i=0; i < maxParamLines; i++) {
        if(noteInstrumentList.at(i)->isChecked()) { // command
            notesList.at(i)->setDisabled(true);
            noteDurationList.at(i)->setDisabled(true);

            commandTypeList.at(i)->setDisabled(false);
            commandXList.at(i)->setDisabled(false);
            commandYList.at(i)->setDisabled(false);
        } else { // note
            notesList.at(i)->setDisabled(false);
            noteDurationList.at(i)->setDisabled(false);

            commandTypeList.at(i)->setDisabled(true);
            commandXList.at(i)->setDisabled(true);
            commandYList.at(i)->setDisabled(true);
        }
    }
}

int PatternEditorWidget::getLineParam(int i)
{
    int word = 0;

    if(noteInstrumentList.at(i)->isChecked()) { // command
        int ID = commandTypeList.at(i)->currentIndex();
        int X = commandXList.at(i)->text().toInt(0, 10);
        int Y = commandYList.at(i)->text().toInt(0, 10);
        if(X > 31) {
            X = 31;
            commandXList.at(i)->setText(QString::number(X));
        }
        if(Y < -16) {
            Y =-16;
            commandYList.at(i)->setText(QString::number(Y));
        }
        if(Y > 15) {
            Y =15;
            commandYList.at(i)->setText(QString::number(Y));
        }

        word = Y+16;
        word <<= 5;
        word += X;
        word <<= 4;
        word += ID;
        word <<= 2;
        word |= 0x01; // set LSB to 1 to indicate it's a command
    } else { // note
        int pitch = notesList.at(i)->currentIndex();
        int duration = noteDurationList.at(i)->text().toInt(0, 10);
        if (duration == 0) return duration;
        if(duration > 255) {
            duration = 255;
            noteDurationList.at(i)->setText(QString::number(duration));
        }

        word = duration;
        word <<= 6;
        word += pitch;
        word <<= 2;
    }

    return word;
}

void PatternEditorWidget::setLineParams(int params, int lineNum)
{
    if(params & 0x01) { // check for command
        params >>= 2;
        int cmd = params & 0x0F;
        params >>= 4;
        int X = params & 0x1F;
        params >>= 5;
        int Y = params - 16;

        noteInstrumentList.at(lineNum)->setChecked(true);
        commandTypeList.at(lineNum)->setCurrentIndex(cmd);
        commandXList.at(lineNum)->setText(QString::number(X));
        commandYList.at(lineNum)->setText(QString::number(Y));

    } else { // nope, it's note
        params >>= 2;
        int pitch = params & 0x003F;
        int duration = params >> 6; // left only duration

        noteInstrumentList.at(lineNum)->setChecked(false);
        notesList.at(lineNum)->setCurrentIndex(pitch);
        noteDurationList.at(lineNum)->setText(QString::number(duration));
    }
}

void PatternEditorWidget::generatePattern(void)
{
    QString output(T_PATTERN_START);
    output.append("patternSample");
    output.append(T_PATTERN_PROGMEM);

    int word = 0;

    for(int i=0; i < maxParamLines; i++) {
        word = getLineParam(i);
        if(word != 0) {
            output.append("0x");
            output.append(QString("%1").arg(word, 4, 16).toUpper().replace(" ", "0").append(","));
        }
    }
    output.append(T_PATTERN_END);

    teResultPattern->setText(output);
}


void PatternEditorWidget::saveFilePattern(void)
{
    QString pathToFile;
    pathToFile.append(QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("GameBuino sound files (*.gbs);;")));
    QFile file(pathToFile);
    QString output(T_FILE_MARK);
    output.append("    "); // future reserv
    output.append(0x01); // version

    int word = 0;

    for(int i=0; i < maxParamLines; i++) {
        word = getLineParam(i);
        if(word != 0) {
            output.append(QString("%1").arg(word, 4, 16).toUpper().replace(" ", "0"));
        }
    }

    if(file.open(QIODevice::ReadWrite)) {

        file.write(output.toStdString().c_str());
    }
    file.close();
}

QStringList PatternEditorWidget::readRawFile(QFile *rawFile)
{
    QString rawData = rawFile->readAll();

    rawData.remove(0,8); // remove file mark, reserve and version

    for(int count=4; count < rawData.count(); count+=5) {
        rawData.insert(count, ",");  // add comma byte spacer
    }
    return rawData.split(",");
}

void PatternEditorWidget::loadFilePattern(void)
{
    QString pathToFile;
    pathToFile.append(QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("GameBuino sound files (*.gbs);;")));
    QFile file(pathToFile);

    QString output(T_PATTERN_START);
    output.append("patternSample");
    output.append(T_PATTERN_PROGMEM);

    if(file.open(QIODevice::ReadOnly)) {
        QStringList paramsList  = readRawFile(&file);

        int word =0;

        for(int i=0; i<paramsList.count(); i++) {
            word = paramsList.at(i).toInt(0, 16);
            setLineParams(word, i);
            output.append("0x" + paramsList.at(i) + ",");
        }
    }

    output.append(T_PATTERN_END);
    teResultPattern->setText(output);
}


// ------------------------------------------------- //
void PatternEditorWidget::setupAudioGenerator(void)
{
    currentNotePos =0;

    //input = new QBuffer();

    format.setSampleRate(SAMPLE_RATE);
    format.setChannelCount(CH_NUMBER);
    format.setSampleSize(BIT_DEPTH);
    format.setCodec(CODEC_TYPE);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    // Create an output with our premade QAudioFormat
    audio = new QAudioOutput(format);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleState(QAudio::State)));
}

bool PatternEditorWidget::checkInstrument(void)
{
    bool state = true;
    while(noteInstrumentList.at(currentNotePos)->isChecked()) {
        if((++currentNotePos) > maxParamLines) {
            currentNotePos =0;
            state = false;
            break;
        }
    }

    return state;
}

void PatternEditorWidget::playPatterns(void)
{
    currentNotePos =0;
    generateSound();
}

void PatternEditorWidget::generateSound(void)
{
    // 2^(1/12) = 1.059463094359295 :: magic number

    bool playNotes = checkInstrument();

    if(notesList.at(currentNotePos)->currentIndex() < NOTES_NUM-1) {
        if(playNotes) {
            // get correct root number
            int power = notesList.at(currentNotePos)->currentIndex() - NOTE_A4_POS;
            // freq calculates as 440 Hz * ((magic number)^power)
            int freq = 440 * pow(1.059463094359295, power);
            // get number of frames and convert to ms (by default each frame == 50 ms)
            int duration = noteDurationList.at(currentNotePos)->text().toInt(0, 10) * FRAME_LONG;

            generateSample(freq, duration);
            playOutput();
        }
    }
}

void PatternEditorWidget::endSound(void)
{
    audio->stop();
    input->close();

    if(bytebuf) delete bytebuf;
    if(input) delete input;
}

void PatternEditorWidget::nextNote(void)
{
    if((++currentNotePos) < maxParamLines) {
        tableEditor->selectRow(currentNotePos-1);
        generateSound();
    } else { // no more notes left
        currentNotePos =0;
    }
}

void PatternEditorWidget::handleState(QAudio::State newState)
{
    switch(newState) {
    case QAudio::IdleState: { // Finished playing (no more data)
        endSound();
        nextNote();
    } break;

    // ... other cases as appropriate
    default: break;
    }
}

void PatternEditorWidget::generateSample(int freq, int milliseconds)
{
    int sampleSize = milliseconds * (SAMPLE_RATE/1000);

    bytebuf = new QByteArray();
    bytebuf->resize(sampleSize);

    for(int i=0; i<sampleSize; i++) {
        (*bytebuf)[i] = (quint8)(qSin((qreal)(freq * i) * FREQ_CONST) * MAX_VOL);
    }
}

void PatternEditorWidget::playOutput(void)
{
    // Make a QBuffer from our QByteArray
    input = new QBuffer(bytebuf);
    //input->setBuffer(bytebuf);
    input->open(QIODevice::ReadOnly);
    audio->start(input);
}

void PatternEditorWidget::deleteTestOutPut(void)
{
    if(audio) delete audio;
    if(bytebuf) delete bytebuf;
    if(input) delete input;
}
