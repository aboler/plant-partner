import 'package:flutter/material.dart';
import 'package:frontend/plant.dart';
import 'package:frontend/task.dart';
import 'package:frontend/services/remote_service.dart';
import 'package:intl/intl.dart';

class TaskView extends StatefulWidget {
  const TaskView({super.key});

  @override
  State<TaskView> createState() => _TaskViewState();
}

class _TaskViewState extends State<TaskView> {
  Plant? plant;
  List<Task> tasks = [];
  bool loading = true;

  @override
  void initState() {
    super.initState();
    loadData();
  }

  Future<void> loadData() async {
    setState(() => loading = true);

    plant = await RemoteService().getPlant();
    tasks = await RemoteService().getTasks();

    setState(() => loading = false);
}

  Future<void> toggleAuto(bool value) async {
    setState(() => loading = true);

    await RemoteService().setAutoSchedule(value);
    await loadData();

    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text(
          value
              ? "Sensor controlled autocare enabled"
              : "Sensor controlled autocare disabled",
        ),
      ),
    );
  }

  Widget buildDayChip(
    String day,
    List<String> selectedDays,
    void Function(void Function()) setDialogState,
  ) {
    final isSelected = selectedDays.contains(day);

    return FilterChip(
      label: Text(day),
      selected: isSelected,
      onSelected: (selected) {
        setDialogState(() {
          if (selected) {
            if (!selectedDays.contains(day)) {
              selectedDays.add(day);
            }
          } else {
            selectedDays.remove(day);
          }
        });
      },
    );
  }

  Widget buildTaskTypeCheckbox(
    String label,
    String value,
    List<String> selectedTypes,
    void Function(void Function()) setDialogState,
  ) {
    return CheckboxListTile(
      contentPadding: EdgeInsets.zero,
      title: Text(label),
      value: selectedTypes.contains(value),
      onChanged: (checked) {
        setDialogState(() {
          if (checked == true) {
            if (!selectedTypes.contains(value)) {
              selectedTypes.add(value);
            }
          } else {
            selectedTypes.remove(value);
          }
        });
      },
    );
  }

  String formatTaskTypes(List<String> taskTypes) {
  if (taskTypes.isEmpty) {
    return "Task";
  }

  final labels = taskTypes.map(taskTypeLabel).toList();
  return labels.join(" + ");
}

  String taskTypeLabel(String type) {
    if (type == "water") return "Water";
    if (type == "light") return "Light";
    if (type == "nutrients") return "Nutrients";
    return type;
  }

  Future<void> addTaskDialog() async {
    List<String> selectedTypes = [];
    DateTime? selectedStartDate;
    DateTime? selectedEndDate;
    TimeOfDay? selectedTime;
    List<String> selectedDays = [];

    await showTaskDialog(
      title: "Add New Task",
      selectedTypes: selectedTypes,
      selectedStartDate: selectedStartDate,
      selectedEndDate: selectedEndDate,
      selectedTime: selectedTime,
      selectedDays: selectedDays,
      onSave: (
        List<String> taskTypes,
        DateTime startDate,
        DateTime endDate,
        TimeOfDay time,
        List<String> repeatDays,
      ) async {
        final startDateStr = DateFormat('yyyy-MM-dd').format(startDate);
        final endDateStr = DateFormat('yyyy-MM-dd').format(endDate);
        final timeStr =
            '${time.hour.toString().padLeft(2, '0')}:${time.minute.toString().padLeft(2, '0')}';

        setState(() => loading = true);

        await RemoteService().createTask(
          taskTypes,
          startDateStr,
          endDateStr,
          timeStr,
          repeatDays,
        );

        await loadData();
      },
    );
  }

  Future<void> editTaskDialog(Task task) async {
    List<String> selectedTypes = List<String>.from(task.taskTypes);
    DateTime? selectedStartDate =
        task.startDate != null ? DateTime.tryParse(task.startDate!) : null;
    DateTime? selectedEndDate =
        task.endDate != null ? DateTime.tryParse(task.endDate!) : null;

    TimeOfDay? selectedTime;
    if (task.time != null && task.time!.contains(':')) {
      final parts = task.time!.split(':');
      if (parts.length == 2) {
        selectedTime = TimeOfDay(
          hour: int.tryParse(parts[0]) ?? 0,
          minute: int.tryParse(parts[1]) ?? 0,
        );
      }
    }

    List<String> selectedDays = List<String>.from(task.repeatDays);

    await showTaskDialog(
      title: "Edit Task",
      selectedTypes: selectedTypes,
      selectedStartDate: selectedStartDate,
      selectedEndDate: selectedEndDate,
      selectedTime: selectedTime,
      selectedDays: selectedDays,
      onSave: (
        List<String> taskTypes,
        DateTime startDate,
        DateTime endDate,
        TimeOfDay time,
        List<String> repeatDays,
      ) async {
        final startDateStr = DateFormat('yyyy-MM-dd').format(startDate);
        final endDateStr = DateFormat('yyyy-MM-dd').format(endDate);
        final timeStr =
            '${time.hour.toString().padLeft(2, '0')}:${time.minute.toString().padLeft(2, '0')}';

        setState(() => loading = true);

        await RemoteService().updateTask(
          task.id,
          taskTypes,
          startDateStr,
          endDateStr,
          timeStr,
          repeatDays,
        );

        await loadData();
      },
    );
  }

  Future<void> showTaskDialog({
    required String title,
    required List<String> selectedTypes,
    required DateTime? selectedStartDate,
    required DateTime? selectedEndDate,
    required TimeOfDay? selectedTime,
    required List<String> selectedDays,
    required Future<void> Function(
      List<String> taskTypes,
      DateTime startDate,
      DateTime endDate,
      TimeOfDay time,
      List<String> repeatDays,
    ) onSave,
  }) async {
    DateTime? tempStartDate = selectedStartDate;
    DateTime? tempEndDate = selectedEndDate;
    TimeOfDay? tempTime = selectedTime;
    List<String> tempTypes = List<String>.from(selectedTypes);
    List<String> tempDays = List<String>.from(selectedDays);

    await showDialog(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setDialogState) {
            return AlertDialog(
              title: Text(title),
              content: SingleChildScrollView(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    const Text(
                      "Task Options",
                      style: TextStyle(fontWeight: FontWeight.bold),
                    ),
                    const SizedBox(height: 8),

                    buildTaskTypeCheckbox(
                      "Water plant",
                      "water",
                      tempTypes,
                      setDialogState,
                    ),
                    buildTaskTypeCheckbox(
                      "Turn on light",
                      "light",
                      tempTypes,
                      setDialogState,
                    ),
                    buildTaskTypeCheckbox(
                      "Add nutrients",
                      "nutrients",
                      tempTypes,
                      setDialogState,
                    ),

                    const SizedBox(height: 16),

                    SizedBox(
                      width: double.infinity,
                      child: OutlinedButton(
                        onPressed: () async {
                          final picked = await showDatePicker(
                            context: context,
                            initialDate: tempStartDate ?? DateTime.now(),
                            firstDate: DateTime.now().subtract(
                              const Duration(days: 365),
                            ),
                            lastDate: DateTime.now().add(
                              const Duration(days: 3650),
                            ),
                          );

                          if (picked != null) {
                            setDialogState(() {
                              tempStartDate = picked;
                              if (tempEndDate != null &&
                                  tempEndDate!.isBefore(tempStartDate!)) {
                                tempEndDate = picked;
                              }
                            });
                          }
                        },
                        child: Text(
                          tempStartDate == null
                              ? "Choose Start Date"
                              : "Start Date: ${DateFormat('MM dd, yyyy').format(tempStartDate!)}",
                        ),
                      ),
                    ),
                    const SizedBox(height: 10),

                    SizedBox(
                      width: double.infinity,
                      child: OutlinedButton(
                        onPressed: () async {
                          final picked = await showDatePicker(
                            context: context,
                            initialDate:
                                tempEndDate ?? tempStartDate ?? DateTime.now(),
                            firstDate: tempStartDate ?? DateTime.now(),
                            lastDate: DateTime.now().add(
                              const Duration(days: 3650),
                            ),
                          );

                          if (picked != null) {
                            setDialogState(() {
                              tempEndDate = picked;
                            });
                          }
                        },
                        child: Text(
                          tempEndDate == null
                              ? "Choose End Date"
                              : "End Date: ${DateFormat('MM dd, yyyy').format(tempEndDate!)}",
                        ),
                      ),
                    ),
                    const SizedBox(height: 10),

                    SizedBox(
                      width: double.infinity,
                      child: OutlinedButton(
                        onPressed: () async {
                          final picked = await showTimePicker(
                            context: context,
                            initialTime: tempTime ?? TimeOfDay.now(),
                          );

                          if (picked != null) {
                            setDialogState(() {
                              tempTime = picked;
                            });
                          }
                        },
                        child: Text(
                          tempTime == null
                              ? "Choose Time"
                              : "Time: ${tempTime!.format(context)}",
                        ),
                      ),
                    ),
                    const SizedBox(height: 16),

                    const Text(
                      "Repeat Days",
                      style: TextStyle(fontWeight: FontWeight.bold),
                    ),
                    const SizedBox(height: 8),

                    Wrap(
                      spacing: 8,
                      runSpacing: 8,
                      children: [
                        buildDayChip("M", tempDays, setDialogState),
                        buildDayChip("T", tempDays, setDialogState),
                        buildDayChip("W", tempDays, setDialogState),
                        buildDayChip("Th", tempDays, setDialogState),
                        buildDayChip("F", tempDays, setDialogState),
                        buildDayChip("Sa", tempDays, setDialogState),
                        buildDayChip("Su", tempDays, setDialogState),
                      ],
                    ),
                  ],
                ),
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.pop(context),
                  child: const Text("Cancel"),
                ),
                ElevatedButton(
                  onPressed: () async {
                    if (tempTypes.isEmpty) {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(
                          content: Text("Please choose at least one task option"),
                        ),
                      );
                      return;
                    }

                    if (tempStartDate == null ||
                        tempEndDate == null ||
                        tempTime == null) {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(
                          content: Text("Please choose start date, end date, and time"),
                        ),
                      );
                      return;
                    }

                    if (tempEndDate!.isBefore(tempStartDate!)) {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(
                          content: Text("End date cannot be before start date"),
                        ),
                      );
                      return;
                    }

                    Navigator.pop(context);

                    await onSave(
                      tempTypes,
                      tempStartDate!,
                      tempEndDate!,
                      tempTime!,
                      tempDays,
                    );
                  },
                  child: const Text("Save"),
                ),
              ],
            );
          },
        );
      },
    );
  }

  Future<void> deleteTask(Task task) async {
    final shouldDelete = await showDialog<bool>(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text("Delete Task"),
          content: Text("Delete ${formatTaskTypes(task.taskTypes)}?"),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context, false),
              child: const Text("Cancel"),
            ),
            TextButton(
              onPressed: () => Navigator.pop(context, true),
              child: const Text(
                "Delete",
                style: TextStyle(color: Colors.red),
              ),
            ),
          ],
        );
      },
    );

    if (shouldDelete != true) return;

    setState(() => loading = true);

    await RemoteService().deleteTask(task.id);
    await loadData();

    ScaffoldMessenger.of(context).showSnackBar(
      const SnackBar(
        content: Text("Task deleted"),
      ),
    );
  }

  String buildCompactSubtitle(Task t) {
    final List<String> lines = [];

    if (t.time != null) {
      lines.add(t.time!);
    }

    if (t.repeatDays.isNotEmpty) {
      lines.add(t.repeatDays.join(", "));
    }

    return lines.join(" | ");
  }

  String buildExpandedSubtitle(Task t) {
    final List<String> lines = [];

    if (t.startDate != null) {
      lines.add("start: ${t.startDate}");
    }
    if (t.endDate != null) {
      lines.add("end: ${t.endDate}");
    }
    if (t.time != null) {
      lines.add("time: ${t.time}");
    }
    if (t.repeatDays.isNotEmpty) {
      lines.add("days: ${t.repeatDays.join(", ")}");
    }

    return lines.join("\n");
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightGreen,
        title: const Text(
          "Care Schedule",
          style: TextStyle(fontWeight: FontWeight.bold),
        ),
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: () async {
              await loadData();

              ScaffoldMessenger.of(context).showSnackBar(
                const SnackBar(
                  content: Text("Tasks refreshed"),
                  duration: Duration(seconds: 1),
                ),
              );
            },
          ),
        ],
      ),
      backgroundColor: Colors.amber.shade50,
      body: loading
          ? const Center(child: CircularProgressIndicator())
          : plant == null
              ? const Center(child: Text("No plant data"))
              : Padding(
                  padding: const EdgeInsets.all(16),
                  child: Column(
                    children: [
                      Card(
                        shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(14),
                        ),
                        child: Padding(
                          padding: const EdgeInsets.all(16),
                          child: Column(
                            mainAxisSize: MainAxisSize.min,
                            children: [
                              const SizedBox(height: 12),
                              SwitchListTile(
                                title: const Text("Sensor Controlled Autocare"),
                                value: plant!.autoSchedule,
                                onChanged: toggleAuto,
                              ),
                            ],
                          ),
                        ),
                      ),
                      const SizedBox(height: 16),
                      Expanded(
                        child: Card(
                          shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(14),
                          ),
                          child: Padding(
                            padding: const EdgeInsets.all(12),
                            child: tasks.isEmpty
                                ? const Center(
                                    child: Text("No tasks yet"),
                                  )
                                : ListView.builder(
                                    itemCount: tasks.length,
                                    itemBuilder: (context, index) {
                                      final t = tasks[index];

                                      return Card(
                                        margin: const EdgeInsets.symmetric(
                                          vertical: 6,
                                        ),
                                        child: ExpansionTile(
                                          title: Text(
                                            formatTaskTypes(t.taskTypes),
                                            style: const TextStyle(
                                              fontWeight: FontWeight.w600,
                                            ),
                                          ),
                                          subtitle: buildCompactSubtitle(t).isEmpty
                                              ? null
                                              : Text(buildCompactSubtitle(t)),
                                          childrenPadding:
                                              const EdgeInsets.fromLTRB(16, 0, 16, 12),
                                          children: [
                                            if (buildExpandedSubtitle(t).isNotEmpty)
                                              Align(
                                                alignment: Alignment.centerLeft,
                                                child: Padding(
                                                  padding: const EdgeInsets.only(bottom: 12),
                                                  child: Text(buildExpandedSubtitle(t)),
                                                ),
                                              ),
                                            Row(
                                              mainAxisAlignment: MainAxisAlignment.end,
                                              children: [
                                                TextButton.icon(
                                                  onPressed: () => editTaskDialog(t),
                                                  icon: const Icon(Icons.edit_outlined),
                                                  label: const Text("Edit"),
                                                ),
                                                TextButton.icon(
                                                  onPressed: () => deleteTask(t),
                                                  icon: const Icon(
                                                    Icons.delete_outline,
                                                    color: Colors.red,
                                                  ),
                                                  label: const Text(
                                                    "Delete",
                                                    style: TextStyle(color: Colors.red),
                                                  ),
                                                ),
                                              ],
                                            ),
                                          ],
                                        ),
                                      );
                                    },
                                  ),
                          ),
                        ),
                      ),
                      const SizedBox(height: 12),
                      SizedBox(
                        width: double.infinity,
                        child: ElevatedButton(
                          style: ElevatedButton.styleFrom(
                            foregroundColor: Colors.black,
                            backgroundColor: Colors.lightGreen,
                          ),
                          onPressed: addTaskDialog,
                          child: const Text("Add New Task"),
                        ),
                      ),
                    ],
                  ),
                ),
    );
  }
}