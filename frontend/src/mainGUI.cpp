// Qt
#include <QApplication>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>
#include <QtCore/qnamespace.h>
#include <QtCore/qobject.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <chrono>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Backend (Student 1)
#include "taskManager.h"
#include "tasks/recurring_task.h"
#include "task.h"
#include "tasks/timed_task.h"

#include "client.hpp"

void refreshTaskList(QListWidget *taskList, const TaskManager &manager,
                     QLineEdit *search, QComboBox *filter, QComboBox *sorting) {

  taskList->clear();
  taskList->blockSignals(true); // these signal block are needet since the code
                                // changes the check marks and this normaly snds
                                // out a signal (but we dont need it now)
  std::string keyword = search->text().toStdString();
  std::vector<Task *> tasks;

  int filterIndex = (filter->currentIndex() < 0) ? 0 : filter->currentIndex();
  int sortIndex = (sorting->currentIndex() < 0) ? 0 : sorting->currentIndex();

  tasks = manager.getTasksForView(keyword, static_cast<FilterMode>(filterIndex),
                                  static_cast<SortMode>(sortIndex));

  for (const Task *task : tasks) {
    // build the text the user sees
    std::string line = task->getTitle() + "  [" + task->getType() + "]";
    if (task->isCompleted())
      line += "  (done)";

    QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(line));
    item->setData(
        Qt::UserRole,
        QString::fromStdString(task->getId())); // stor the id in the UserRole
                                                // slot (kind of like a map)
    // turn the checkbox ON for this item
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(task->isCompleted() ? Qt::Checked : Qt::Unchecked);

    taskList->addItem(item);
  }
  taskList->blockSignals(false);
};

int main(int argc, char *argv[]) {
  
  std::string userid = "user";
  if (argc > 1) {
    userid = argv[1];
  }

  std::shared_ptr<TaskClient> client(std::make_shared<TaskClient>("127.0.0.1", 8080, userid));
  QApplication app(argc, argv);

  // usefull variables

  QStringList priorityQstrList = {"Low", "Medium", "High"};
  QStringList filterQstrList = {
      "All",       "Priority Low", "Priority Medium", "Priority High",
      "Completed", "Overdue",      "Recurring",       "Timed"};
  QStringList categoryQstrList = {"Work",   "Education", "Personal",
                                  "Health", "Finance",   "Other"};
  QStringList recurrenceQstrList = {"None", "Daily", "Weekly", "Monthly",
                                    "Yearly"};
  QStringList sortingQstrList = {"Deadline", "Priority"};

  int descrHeight = 120;
  int labelHeight = 50;

  std::string editingId;

  // --------------------------------------------------------
  // HOME SCREEN
  // --------------------------------------------------------

  // === HOME WIDGETS ===

  QLineEdit *search = new QLineEdit();
  search->setPlaceholderText("Search by title...");

  QComboBox *filter = new QComboBox();
  filter->addItems(filterQstrList);
  filter->setCurrentIndex(0);

  QComboBox *sorting = new QComboBox();
  sorting->addItems(sortingQstrList);
  sorting->setCurrentIndex(0);

  QHBoxLayout *searchComboBox = new QHBoxLayout;
  searchComboBox->addWidget(filter);
  searchComboBox->addWidget(sorting);

  QListWidget *taskList = new QListWidget(); // itemms addet later in connect

  QPushButton *infoButtton = new QPushButton("Info");
  QPushButton *deleteButton = new QPushButton("Delete");
  QPushButton *editButton = new QPushButton("Edit");
  QPushButton *addTaskButton = new QPushButton("Add task");

  QHBoxLayout *homeButtonsBox = new QHBoxLayout();
  homeButtonsBox->addWidget(infoButtton);
  homeButtonsBox->addWidget(deleteButton);
  homeButtonsBox->addWidget(editButton);

  // === HOME LAYOUTS ===

  QFormLayout *homeScreen = new QFormLayout;
  homeScreen->addRow(search);
  homeScreen->addRow(searchComboBox);
  homeScreen->addRow(taskList);
  homeScreen->addRow(homeButtonsBox);
  homeScreen->addRow(addTaskButton);

  QWidget *window = new QWidget;
  window->setLayout(homeScreen);
  window->setWindowTitle(QString::fromStdString("Todolist App — " + userid));
  window->show();

  // === home connencts ===

  TaskManager manager(client);
  manager.setOnChange([&]() {
      QMetaObject::invokeMethod(window, [&]() {
          refreshTaskList(taskList, manager, search, filter, sorting);
      }, Qt::QueuedConnection);
  });
  // apply changes pushed by other clients without re-sending to server
  client->setNotifyCallback([&](Type type, std::vector<std::unique_ptr<Task>> tasks) {
      QMetaObject::invokeMethod(window, [&manager, &taskList, &search, &filter, &sorting,
                                          type, tasks = std::move(tasks)]() mutable {
          if (type == Type::NOTIFY) {
              // show reminder dialog for each due task
              for (auto& t : tasks) {
                  if (!t) continue;
                  QMessageBox* box = new QMessageBox();
                  box->setWindowTitle("Reminder");
                  box->setText(QString("Task due: <b>%1</b>")
                      .arg(QString::fromStdString(t->getTitle())));
                  if (!t->getDescription().empty())
                      box->setInformativeText(QString::fromStdString(t->getDescription()));
                  box->setIcon(QMessageBox::Information);
                  box->setAttribute(Qt::WA_DeleteOnClose);
                  box->show();
              }
              refreshTaskList(taskList, manager, search, filter, sorting);
              return;
          }

          for (auto& t : tasks) {
              if (!t) continue;
              if (type == Type::ADD)         manager.applyAdd(std::move(t));
              else if (type == Type::UPDATE)  manager.applyUpdate(std::move(t));
              else if (type == Type::DELETE)  manager.applyDelete(t->getId());
          }
          refreshTaskList(taskList, manager, search, filter, sorting);
      }, Qt::QueuedConnection);
  });

  manager.loadTasks();

  // startup:

  //refreshTaskList(taskList, manager, search, filter, sorting);

  // delete button
  QObject::connect(
      deleteButton, &QPushButton::clicked,
      [taskList, &manager, search, filter, sorting]() {
        // 1. get the selected item
        QListWidgetItem *selected = taskList->currentItem(); // nullptr if none
        // 2. guard: what if nothing is selected?
        if (selected) {
          // 3. read its ID from Qt::UserRole
          std::string taskId =
              selected->data(Qt::UserRole).toString().toStdString();
          // 4. tell the manager to delete it
          manager.deleteTask(taskId);
          // 6. refresh the list
          refreshTaskList(taskList, manager, search, filter, sorting);
        }
      });

  QObject::connect(search, &QLineEdit::textChanged,
                   [taskList, &manager, search, filter, sorting]() {
                     refreshTaskList(taskList, manager, search, filter,
                                     sorting);
                   });
  QObject::connect(filter, &QComboBox::currentIndexChanged,
                   [taskList, &manager, search, filter, sorting]() {
                     refreshTaskList(taskList, manager, search, filter,
                                     sorting);
                   });
  QObject::connect(sorting, &QComboBox::currentIndexChanged,
                   [taskList, &manager, search, filter, sorting]() {
                     refreshTaskList(taskList, manager, search, filter,
                                     sorting);
                   });
  QObject::connect(
      taskList, &QListWidget::itemChanged,
      [taskList, &manager, &client, search, filter, sorting](QListWidgetItem *item) {
        // 1. read the task ID from the item (Qt::UserRole — same as always)
        std::string taskId = item->data(Qt::UserRole).toString().toStdString();
        Task *task = manager.searchById(taskId);

        if (task) {
          // 2. set that task's completed state based on the checkbox
          task->setCompleted(item->checkState() == Qt::Checked);
          // 3. send update to server
          client->updateTask(*task);
          // 4. refresh
          refreshTaskList(taskList, manager, search, filter, sorting);
        }
      });

  // --------------------------------------------------------
  // INFO SCREEN
  // --------------------------------------------------------

  // === info widgets ===

  QLabel *titleLabel = new QLabel("Title:");
  QLineEdit *titleInfo = new QLineEdit;
  titleInfo->setReadOnly(true);

  QLabel *descriptionLabel = new QLabel("Description:");
  QTextEdit *descriptionInfo = new QTextEdit;
  descriptionInfo->setReadOnly(true);
  descriptionInfo->setFixedHeight(descrHeight);

  QLabel *labelLabel = new QLabel("Label:");
  QTextEdit *labelInfo = new QTextEdit;
  labelInfo->setReadOnly(true);
  labelInfo->setFixedHeight(labelHeight);

  QLineEdit *deadlineInfo = new QLineEdit;
  deadlineInfo->setReadOnly(true);

  QLineEdit *priorityInfo = new QLineEdit;
  priorityInfo->setReadOnly(true);
  QLineEdit *categoryInfo = new QLineEdit;
  categoryInfo->setReadOnly(true);
  QLineEdit *recurrenceInfo = new QLineEdit;
  recurrenceInfo->setReadOnly(true);

  // === info layouts ===

  QFormLayout *infoScreen = new QFormLayout;
  infoScreen->addRow(titleLabel);
  infoScreen->addRow(titleInfo);
  infoScreen->addRow(descriptionLabel);
  infoScreen->addRow(descriptionInfo);
  infoScreen->addRow(labelLabel);
  infoScreen->addRow(labelInfo);
  infoScreen->addRow("Deadline:", deadlineInfo);
  infoScreen->addRow("Priority:", priorityInfo);
  infoScreen->addRow("Category:", categoryInfo);
  infoScreen->addRow("Recurrence:", recurrenceInfo);

  QDialog *infoDialog = new QDialog;
  infoDialog->setLayout(infoScreen);
  infoDialog->setWindowTitle("Info");

  // === info connects ===

  // info button
  QObject::connect(infoButtton, &QPushButton::clicked, [=, &manager]() {
    // 1. get the selected item
    QListWidgetItem *selected = taskList->currentItem(); // nullptr if none
    // 2. guard: what if nothing is selected?
    if (selected) { // check if selected == nulpointer if it does the {} is
                    // skipped
      // 3. read its ID from Qt::UserRole
      std::string taskId =
          selected->data(Qt::UserRole).toString().toStdString();
      // 4. search for the task the id
      Task *task = manager.searchById(taskId);
      if (task) { // if task != nullptr -> set the Info window values

        // access different elements from the task object and display them
        titleInfo->setText(QString::fromStdString(task->getTitle()));
        descriptionInfo->setPlainText(
            QString::fromStdString(task->getDescription()));

        std::time_t time =
            std::chrono::system_clock::to_time_t(task->getDeadline());
        std::stringstream deadlineStrStream;
        deadlineStrStream << std::put_time(std::localtime(&time),
                                           "%Y-%m-%d %H:%M");
        deadlineInfo->setText(QString::fromStdString(deadlineStrStream.str()));

        std::vector<std::string> labelVec = task->getLabel();
        std::stringstream labelStrStream;
        for (const auto &label :
             labelVec) { // const auto& - to read each label without copying
          labelStrStream << label << " ";
        }
        labelInfo->setText(QString::fromStdString(labelStrStream.str()));

        int priorityIndex = static_cast<int>((task->getPriority()));
        priorityInfo->setText(priorityQstrList[priorityIndex]);

        int categoryIndex = static_cast<int>((task->getCategory()));
        categoryInfo->setText(categoryQstrList[categoryIndex]);

        int recurrenceIndex = static_cast<int>((task->getRecurrence()));
        recurrenceInfo->setText(recurrenceQstrList[recurrenceIndex]);

        infoDialog->show();
      }
    }
  });

  // --------------------------------------------------------
  // EDIT SCREEN
  // --------------------------------------------------------

  // === edit widgets ===

  QPushButton *doneButton = new QPushButton("Done");
  QPushButton *cancelButton = new QPushButton("Cancel");

  QLineEdit *titleEdit = new QLineEdit;
  QLabel *titleLabelEdit = new QLabel("Title:");
  QHBoxLayout *titleLyaout = new QHBoxLayout;
  titleLyaout->addWidget(titleLabelEdit);
  titleLyaout->addWidget(titleEdit);

  QTextEdit *descriptionEdit = new QTextEdit;
  descriptionEdit->setFixedHeight(descrHeight);

  QTextEdit *labelEdit = new QTextEdit;
  labelEdit->setFixedHeight(labelHeight);

  QDateTimeEdit *deadlineEdit = new QDateTimeEdit;
  deadlineEdit->setCalendarPopup(true);

  QComboBox *priority = new QComboBox;
  priority->addItems(priorityQstrList);

  QComboBox *category = new QComboBox;
  category->addItems(categoryQstrList);

  QComboBox *recurrence = new QComboBox;
  recurrence->addItems(recurrenceQstrList);

  // === edit layouts ===

  QFormLayout *editScreen = new QFormLayout;
  editScreen->addRow(titleLyaout);
  editScreen->addRow("Description:", descriptionEdit);
  editScreen->addRow("Label:", labelEdit);
  editScreen->addRow("Deadline:", deadlineEdit);
  editScreen->addRow("Priority:", priority);
  editScreen->addRow("Category:", category);
  editScreen->addRow("Recurrence:", recurrence);
  editScreen->addRow(doneButton, cancelButton);
  // editScreen->setFieldGrowthPolicy(
  //   QFormLayout::AllNonFixedFieldsGrow); // such that the right lineEdit
  //  element expands as the window
  //  grows

  QDialog *editDialog = new QDialog;
  editDialog->setLayout(editScreen);
  editDialog->setWindowTitle("Edit");
  // editDialog->show();

  // === edit connects ===

  QObject::connect(cancelButton, &QPushButton::clicked,
                   [editDialog]() { editDialog->hide(); });

  QObject::connect(addTaskButton, &QPushButton::clicked,
                   [titleEdit, descriptionEdit, labelEdit, deadlineEdit,
                    priority, category, recurrence, editDialog, &editingId]() {
                     editingId = "";
                     titleEdit->clear();
                     descriptionEdit->clear();
                     labelEdit->clear();
                     deadlineEdit->setDateTime(QDateTime::currentDateTime());
                     priority->setCurrentIndex(0);
                     category->setCurrentIndex(0);
                     recurrence->setCurrentIndex(0);

                     editDialog->show();
                   });

  QObject::connect(
      editButton, &QPushButton::clicked, [=, &manager, &editingId]() {
        QListWidgetItem *selected = taskList->currentItem();

        if (selected) {
          editingId = selected->data(Qt::UserRole).toString().toStdString();
          Task *task = manager.searchById(editingId);

          if (task) {

            titleEdit->setText(QString::fromStdString(task->getTitle()));
            descriptionEdit->setPlainText(
                QString::fromStdString(task->getDescription()));

            // deadline
            // setting (backend):
            deadlineEdit->setDateTime(QDateTime::fromSecsSinceEpoch(
                std::chrono::system_clock::to_time_t(task->getDeadline())));

            std::vector<std::string> labelVec = task->getLabel();
            std::stringstream labelStrStream;
            for (const auto &label :
                 labelVec) { // const auto& - to read each label without copying
              labelStrStream << label << " ";
            }
            labelEdit->setText(QString::fromStdString(labelStrStream.str()));

            int priorityIndex = static_cast<int>((task->getPriority()));
            priority->setCurrentIndex(priorityIndex);

            int categoryIndex = static_cast<int>((task->getCategory()));
            category->setCurrentIndex(categoryIndex);

            int recurrenceIndex = static_cast<int>((task->getRecurrence()));
            recurrence->setCurrentIndex(recurrenceIndex);

            editDialog->show();
          }
        }
      });

  QObject::connect(
      doneButton, &QPushButton::clicked,
      [titleEdit, descriptionEdit, labelEdit, deadlineEdit, priority, category,
       recurrence, editDialog, &editingId, &manager, taskList, search, filter,
       sorting]() {
        std::string title = titleEdit->text().toStdString();
        std::string description = descriptionEdit->toPlainText().toStdString();

        std::vector<std::string> labels;
        std::stringstream labelStrStream(
            labelEdit->toPlainText().toStdString());
        std::string word;
        while (labelStrStream >> word)
          labels.push_back(word);

        // reading (edit → backend):  QDateTime -> time_point
        auto deadline = std::chrono::system_clock::from_time_t(
            deadlineEdit->dateTime().toSecsSinceEpoch());

        Priority p = static_cast<Priority>(priority->currentIndex());
        Category c = static_cast<Category>(category->currentIndex());
        Recurrence r = static_cast<Recurrence>(recurrence->currentIndex());

        auto task = std::make_unique<Task>(title, deadline, p, c, r);
        task->setDescription(description);
        for (const std::string &label : labels)
          task->setLabel(label);

        if (editingId.empty()) {
          manager.addTask(std::move(task));
        } else {
          task->setId(editingId);
          // preserve completed state from original task
          Task *original = manager.searchById(editingId);
          if (original) task->setCompleted(original->isCompleted());
          manager.updateTask(editingId, std::move(task));
        }

        refreshTaskList(taskList, manager, search, filter, sorting);
        editDialog->hide();
      });

  return app.exec();
}
