#ifndef _PATTERNEDITORWIDGET_H
#define _PATTERNEDITORWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListView>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QFileDialog>
#include <QTableWidget>


#include <QWidget>
#include <QAudio>
#include <QAudioOutPut>
#include <QBuffer>
#include <qmath.h>
#include <cmath>

class PatternEditorWidget : public QWidget
{
    Q_OBJECT

public:
    PatternEditorWidget(QWidget *parent = 0);
    ~PatternEditorWidget();

    void addNewLine(void);
    void addFieldDescription(void);
    void deleteParamLines(void);
    void makeTableEditor(void);

    int getLineParam(int id);
    void setLineParams(int params, int lineNum);
    QStringList readRawFile(QFile *rawFile );

    QGridLayout *gInputLayout;

    QTextEdit *teResultPattern;

    QTableWidget *tableEditor;

    QList<QCheckBox*> noteInstrumentList;
    QList<QComboBox*> notesList;
    QList<QLineEdit*> noteDurationList;
    QList<QComboBox*> commandTypeList;
    QList<QLineEdit*> commandXList;
    QList<QLineEdit*> commandYList;


// ------------------------------------------------- //
    int currentNotePos;

    QAudioOutput *audio;
    QByteArray *bytebuf;
    QBuffer *input;

    QAudioFormat format;

    void setupAudioGenerator(void);
    void generateSample(int freq, int milliseconds);
    void playOutput(void);
    void deleteTestOutPut(void);

    void generateSound(void);
    bool checkInstrument(void);
    void nextNote(void);
    void endSound(void);
// ------------------------------------------------- //

private slots:
    void generatePattern(void);
    void checkBoxChanged(void);

    void saveFilePattern(void);
    void loadFilePattern(void);

// ------------------------------------------------- //
    void handleState(QAudio::State newState);
    void playPatterns(void);
// ------------------------------------------------- //

};

#endif // _PATTERNEDITORWIDGET_H
