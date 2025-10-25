import 'package:flutter/material.dart';

class TaskView extends StatelessWidget {
  const TaskView({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightGreen,
        title: const Text("Tasks"),
      ),
      backgroundColor: Colors.amber.shade50,
    );
  }
}