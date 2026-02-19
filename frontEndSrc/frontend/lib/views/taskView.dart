import 'package:flutter/material.dart';
import 'package:frontend/plant.dart';
import 'package:frontend/task.dart';
import 'package:frontend/services/remote_service.dart';

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

    // reload so UI matches database
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

    await showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text("Add New Task"),
          content: DropdownButton<String>(
            value: selectedType,
            items: const [
              DropdownMenuItem(
                value: "water",
                child: Text("Water plant"),
              ),
            ],
            onChanged: (val) {
              if (val != null) selectedType = val;
            },
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text("Cancel"),
            ),
            ElevatedButton(
              onPressed: () async {
                Navigator.pop(context);

                setState(() => loading = true);

                // creates a task in the database
                await RemoteService().createTask(selectedType);

                // reload list
                await loadData();
              },
              child: const Text("Save"),
            ),
          ],
        );
      },
    );
  }

  Future<void> markDone(Task task) async {
    setState(() => loading = true);

    await RemoteService().markTaskDone(task.id);

    // reload list
    await loadData();
  }

  String taskLabel(String type) {
    if (type == "water") return "Water plant";
    return type;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightGreen,
        title: const Text("Care Schedule", style: TextStyle(fontWeight: FontWeight.bold)),
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
                                        subtitle: Text(done ? "done" : "pending"),
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
                            backgroundColor: Colors.lightGreen),
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
