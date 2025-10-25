import 'package:flutter/material.dart';

class DashView extends StatelessWidget {
  const DashView({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightGreen,
        title: const Text("Dashboard"),
      ),
    );
  }
}