#include "logfileanalyser.h"

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>

#include "LogData/LogEntryParser_dummy.h"
#include "LogData/LogEntryFactory.h"
#include "LogData/ParserStreamGetterFile.h"

#include "Models/LogEntryTableModel.h"
#include "LogEntryTableWindow.h"

#include "Models/StringCacheTreeModel.h"
#include "LogData/LogEntryParser_Logfile.h"
#include "LogData/LogEntryParser_LogfileLWI.h"
#include "LogData/LogEntryParser_log4cplusSocket.h"
#include "GUITools/WidgetStateSaver.h"
#include "GUITools/GetSetStateSaver.hxx"
#include "GUITools/SignalMultiplexerStateApplier.h"
#include "ActionRules/FilterRuleSelectionWindow.h"
#include "Help/HelpAssistant.h"



Log4cplusGUIIntegration::Log4cplusGUIIntegration( ParserActionInterface* parserActionIfc )
: m_parserActionIfc(parserActionIfc)
{
	m_toolbar = new QToolBar("Log4cplus v2.2");

	QAction* actionOpenServer = new QAction(this);
	actionOpenServer->setObjectName(QStringLiteral("actionOpenLog4cplusServer"));
	QIcon icon1;
	icon1.addFile(QStringLiteral(":/icons/network"), QSize(), QIcon::Normal, QIcon::On);
	actionOpenServer->setIcon(icon1);

	actionOpenServer->setText(QApplication::translate("Plugin_Source_Log4cplus", "Start Server", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
	actionOpenServer->setToolTip(QApplication::translate("Plugin_Source_Log4cplus", "Starts the Log4cplus logging server.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP

	QLabel *portLabel = new QLabel(QApplication::translate("Plugin_Source_Log4cplus", "Port: ", Q_NULLPTR));
	m_port = new QSpinBox(m_toolbar);
	m_port->setToolTip(QApplication::translate("Plugin_Source_Log4cplus", "Port to listen for log4cplus socket appender", Q_NULLPTR));
	m_port->setMinimum(1);
	m_port->setMaximum(65535);
	m_port->setValue(9998);

	m_toolbar->addAction(actionOpenServer);
	m_toolbar->addWidget(portLabel);
	m_toolbar->addWidget(m_port);

	QObject::connect(actionOpenServer, SIGNAL(triggered()), this, SLOT(openPort()));
}

Log4cplusGUIIntegration::~Log4cplusGUIIntegration()
{
	delete m_toolbar;
}

void Log4cplusGUIIntegration::openPort()
{
  int port = m_port->value();
  boost::shared_ptr<LogEntryParser_log4cplusSocket> socketParser(
    new LogEntryParser_log4cplusSocket(port));

  m_parserActionIfc->newParser(socketParser);
}

LogfileAnalyser::LogfileAnalyser(QWidget *parent)
  : QMainWindow(parent),
    m_myFilterDock( NULL),
    m_stateSaver( NULL),
    m_helpAssistant(new HelpAssistant)
{
  ui.setupUi(this);

  m_log4cplusIntegration = new Log4cplusGUIIntegration(this);
  addToolBar( m_log4cplusIntegration->getToolbar() );

  m_stateSaver = new WidgetStateSaver(this);
  m_stateSaver->addElementToWatch(&m_signalMultiplexer,
                                  SignalMultiplexerStateApplier::generate(&m_signalMultiplexer));

  QObject::connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(showDocumentation()));
  QObject::connect(ui.actionOpenDummyLogger, SIGNAL(triggered()), this, SLOT(openDummyLogfile()));
  QObject::connect(ui.actionAddEntries, SIGNAL(triggered()), this, SLOT(moreDummyLogfile()));
  QObject::connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openLogfile()));
  QObject::connect(ui.actionExportLogfile, SIGNAL(triggered()), this, SLOT(exportLogfile()));
  QObject::connect(ui.mdiArea, SIGNAL(subWindowActivated ( QMdiSubWindow *)), this,
                   SLOT(subWindowActivated( QMdiSubWindow * )));

  m_myFilterRulesDock = new QDockWidget(tr("Filter Rules"), this);
  m_myFilterRulesDock->setAllowedAreas(Qt::AllDockWidgetAreas);
  m_myfilterRuleSelectionWidget = new FilterRuleSelectionWindow(m_myFilterRulesDock);
  m_myFilterRulesDock->setWidget(m_myfilterRuleSelectionWidget);
  addDockWidget(Qt::RightDockWidgetArea, m_myFilterRulesDock);

  m_stateSaver->addElementToWatch(m_myfilterRuleSelectionWidget,
                                  GetSetStateSaver<FilterRuleSelWndStateSaverTypes>::generate());
  m_stateSaver->addElementToWatch(ui.actionCapture,
                                  GetSetStateSaver<QQctionCheckedSaverTypes>::generate());

}

void LogfileAnalyser::subWindowDestroyed(QObject *obj)
{
  qDebug() << "subWindowDestroyed";
  // Window closed, remove docks
  if (ui.mdiArea->subWindowList().size() == 0)
    m_stateSaver->switchState( NULL);
  else
  {
    QMdiSubWindow *wnd = ui.mdiArea->activeSubWindow();
    if (wnd)
      m_stateSaver->switchState(wnd);
    else
      m_stateSaver->switchState(ui.mdiArea->subWindowList().front());
  }

  m_stateSaver->deregisterFocusObject(obj, false);
}

void LogfileAnalyser::subWindowActivated(QMdiSubWindow *obj)
{
  qDebug() << "subWindowActivated";
  if (obj != NULL)
    m_stateSaver->switchState(obj);
}

void LogfileAnalyser::showDocumentation()
{
  qDebug() << "ShuwDocumentation";

  m_helpAssistant->showDocumentation("index.html");
}

LogfileAnalyser::~LogfileAnalyser()
{
	delete m_log4cplusIntegration;
}

void LogfileAnalyser::openLogfile(const QString &fileName)
{
  boost::shared_ptr<LogEntryParser> parser(
      new LogEntryParser_LogfileLWI(fileName));
  if (!parser->initParser())
  {
    qDebug() << " LWI-Parser failed: " << parser->getInitError();
    parser.reset(
        new LogEntryParser_Logfile(
            boost::make_shared<ParserStreamGetterFile>(fileName)));
    if (parser->initParser())
      createWindowsFromParser(parser, true);
    else
    {
      QMessageBox msgBox;
      QString errorText;
      errorText += tr("Error while initializing parser: \n")
          + parser->getInitError();
      msgBox.setText(errorText);
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
    }
  }
  else
  {
    createWindowsFromParser(parser, true);
  }
}

void LogfileAnalyser::openLogfile()
{
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec())
  {
    QStringList fileNames = dialog.selectedFiles();
    openLogfile(fileNames.first());
  }
}

void LogfileAnalyser::newParser(boost::shared_ptr<LogEntryParser> parser, bool alreadyInitialized)
{
	createWindowsFromParser( parser, alreadyInitialized);
}

void LogfileAnalyser::createWindowsFromParser(boost::shared_ptr<LogEntryParser> parser, bool alreadyInitialized)
{
  if (!alreadyInitialized && !parser->initParser())
  {
    // Parser has an error while init, so view message box to inform user and do
    // not create a new window.
    QMessageBox msgBox;
    QString errorText;
    errorText += tr("Error while initializing parser: \n") + parser->getInitError();
    msgBox.setText(errorText);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return;
  }

  boost::shared_ptr<LogEntryTableModel> model(new LogEntryTableModel(parser));

  LogEntryTableWindow *wnd = new LogEntryTableWindow(model, ui.mdiArea);
  m_signalMultiplexer.setObject(wnd);
  m_signalMultiplexer.connect(ui.actionClearLogTable, SIGNAL(triggered()), wnd, SLOT(clearTable()));
  m_signalMultiplexer.connect(ui.actionCapture, SIGNAL(toggled(bool)), wnd, SLOT(capture(bool)));
  m_signalMultiplexer.connect(ui.actionAddQuicksearchFilter, SIGNAL(triggered()), wnd, SLOT(addQuicksearchBar()));

  wnd->setWindowState(Qt::WindowMaximized);
  wnd->setAttribute(Qt::WA_DeleteOnClose);
  wnd->setWindowTitle(parser->getName());
  wnd->show();

  ui.actionCapture->setChecked(true);

  /*
   * We want to open the Dock the first time we create a window.
   * The advantage of doing so is the correct size for the inner
   * filter tab widget which is set to the dock widget.
   */
  if (!m_myFilterDock)
  {
    m_myFilterDock = new QDockWidget(tr("FilterSettings"), this);
    m_myFilterDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_stateSaver->addElementToWatch(m_myFilterDock, DockWidgetStateSaver::generate());

    wnd->setDockForFilter(m_myFilterDock);
    addDockWidget(Qt::RightDockWidgetArea, m_myFilterDock);
  }
  else
  {
    wnd->setDockForFilter(m_myFilterDock);
  }

  QObject::connect(wnd, SIGNAL(destroyed ( QObject *)), this,
                   SLOT(subWindowDestroyed( QObject * )));

  model->startModel();

  TSharedCompiledRulesStateSaver state = wnd->getCompiledRules();
  m_myfilterRuleSelectionWidget->setWindow(state);

}

void LogfileAnalyser::openDummyLogfile()
{
  // Create table with log entries and a new model for this
  boost::shared_ptr<LogEntryParser_dummy> parser(new LogEntryParser_dummy);
  m_parser = parser;

  createWindowsFromParser(parser);
}

void LogfileAnalyser::moreDummyLogfile()
{
  if (!m_parser)
    return;

  m_parser->addEntries(100);
}

void LogfileAnalyser::exportLogfile()
{
  LogEntryTableWindow *wnd = NULL;
  wnd = dynamic_cast<LogEntryTableWindow *>(ui.mdiArea->activeSubWindow());

  if (wnd)
  {
    QString fname;
    fname += "exportedLogfile " + QDateTime::currentDateTime().toString("yyyyMMdd hhmm") + ".log";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Logfile"), fname,
                                                    tr("Logfile (*.log *.txt)"));

    if (fileName.length())
      wnd->exportLogfile(fileName);
  }
}
