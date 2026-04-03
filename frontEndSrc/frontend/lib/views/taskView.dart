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
          value ? "Autocare enabled" : "Autocare disabled",
        ),
      ),
    );
  }

  Future<void> addTaskDialog() async {
    String selectedType = "water";
    DateTime? selectedStartDate;
    DateTime? selectedEndDate;
    TimeOfDay? selectedTime;

    await showDialog(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setDialogState) {
            return AlertDialog(
              title: const Text("Add New Task"),
              content: SingleChildScrollView(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    DropdownButtonFormField<String>(
                      value: selectedType,
                      decoration: const InputDecoration(
                        labelText: "Task Type",
                        border: OutlineInputBorder(),
                      ),
                      items: const [
                        DropdownMenuItem(
                          value: "water",
                          child: Text("Water plant"),
                        ),
                        DropdownMenuItem(
                          value: "light",
                          child: Text("Turn on light"),
                        ),
                        DropdownMenuItem(
                          value: "nutrients",
                          child: Text("Add nutrients"),
                        ),
                      ],
                      onChanged: (val) {
                        if (val != null) {
                          setDialogState(() {
                            selectedType = val;
                          });
                        }
                      },
                    ),
                    const SizedBox(height: 16),

                    SizedBox(
                      width: double.infinity,
                      child: OutlinedButton(
                        onPressed: () async {
                          final picked = await showDatePicker(
                            context: context,
                            initialDate: selectedStartDate ?? DateTime.now(),
                            firstDate: DateTime.now().subtract(const Duration(days: 365)),
                            lastDate: DateTime.now().add(const Duration(days: 3650)),
                          );

                          if (picked != null) {
                            setDialogState(() {
                              selectedStartDate = picked;

                              if (selectedEndDate != null &&
                                  selectedEndDate!.isBefore(selectedStartDate!)) {
                                selectedEndDate = picked;
                              }
                            });
                          }
                        },
                        child: Text(
                          selectedStartDate == null
                              ? "Choose Start Date"
                              : "Start Date: ${DateFormat('MMM d, yyyy').format(selectedStartDate!)}",
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
                            initialDate: selectedEndDate ??
                                selectedStartDate ??
                                DateTime.now(),
                            firstDate: selectedStartDate ?? DateTime.now(),
                            lastDate: DateTime.now().add(const Duration(days: 3650)),
                          );

                          if (picked != null) {
                            setDialogState(() {
                              selectedEndDate = picked;
                            });
                          }
                        },
                        child: Text(
                          selectedEndDate == null
                              ? "Choose End Date"
                              : "End Date: ${DateFormat('MMM d, yyyy').format(selectedEndDate!)}",
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
                            initialTime: selectedTime ?? TimeOfDay.now(),
                          );

                          if (picked != null) {
                            setDialogState(() {
                              selectedTime = picked;
                            });
                          }
                        },
                        child: Text(
                          selectedTime == null
                              ? "Choose Time"
                              : "Time: ${selectedTime!.format(context)}",
                        ),
                      ),
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
                    if (selectedStartDate == null ||
                        selectedEndDate == null ||
                        selectedTime == null) {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(
                          content: Text("Please choose start date, end date, and time"),
                        ),
                      );
                      return;
                    }

                    if (selectedEndDate!.isBefore(selectedStartDate!)) {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(
                          content: Text("End date cannot be before start date"),
                        ),
                      );
                      return;
                    }

                    Navigator.pop(context);

                    setState(() => loading = true);

                    final startDateStr =
                        DateFormat('yyyy-MM-dd').format(selectedStartDate!);
                    final endDateStr =
                        DateFormat('yyyy-MM-dd').format(selectedEndDate!);

                    final timeStr =
                        '${selectedTime!.hour.toString().padLeft(2, '0')}:${selectedTime!.minute.toString().padLeft(2, '0')}';

                    await RemoteService().createTask(
                      selectedType,
                      startDateStr,
                      endDateStr,
                      timeStr,
                    );

                    await loadData();
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

  Future<void> markDone(Task task) async {
    setState(() => loading = true);

    await RemoteService().markTaskDone(task.id);

    await loadData();
  }

  String taskLabel(String type) {
    if (type == "water") return "Water plant";
    if (type == "light") return "Turn on light";
    if (type == "nutrients") return "Add nutrients";
    return type;
  }

  String buildTaskSubtitle(Task t) {
    final done = t.status == "done";

    final List<String> lines = [];
    lines.add(done ? "done" : "pending");

    if (t.startDate != null) {
      lines.add("start: ${t.startDate}");
    }
    if (t.endDate != null) {
      lines.add("end: ${t.endDate}");
    }
    if (t.time != null) {
      lines.add("time: ${t.time}");
    }

    return lines.join(" • ");
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
            onPressed: loadData,
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
                                title: const Text("Enable Autocare"),
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
                                      final done = t.status == "done";

                                      return ListTile(
                                        title: Text(taskLabel(t.type)),
                                        subtitle: Text(buildTaskSubtitle(t)),
                                        trailing: done
                                            ? const Icon(Icons.check, color: Colors.green)
                                            : IconButton(
                                                icon: const Icon(Icons.check_circle_outline),
                                                onPressed: () => markDone(t),
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