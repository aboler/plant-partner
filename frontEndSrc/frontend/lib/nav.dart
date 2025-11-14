import 'package:flutter/material.dart';
import 'package:frontend/views/homeView.dart';
import 'package:frontend/views/settingsView.dart';
import 'package:frontend/views/taskView.dart';

class Nav extends StatefulWidget {
  const Nav({super.key});

  @override
  State<Nav> createState() => _NavState();
}

class _NavState extends State<Nav> {
  int _selectedIndex = 0;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      bottomNavigationBar: NavigationBar(
        selectedIndex: _selectedIndex,
        onDestinationSelected: (int index) {
          setState(() {
            _selectedIndex = index;
          });
        },
        destinations: const <NavigationDestination>[
          NavigationDestination(
            selectedIcon: Icon(Icons.calendar_month),
            icon: Icon(Icons.calendar_month_outlined),
            label: 'Tasks',
          ),
          NavigationDestination(
            selectedIcon: Icon(Icons.home),
            icon: Icon(Icons.home_outlined),
            label: 'Home',
          ),
          NavigationDestination(
            selectedIcon: Icon(Icons.settings_applications),
            icon: Icon(Icons.settings),
            label: 'Settings',
          ),
        ],
      ),

      //body
      body: SafeArea(
        child: IndexedStack(
          index: _selectedIndex,
          children: const <Widget>[
            TaskView(),
            HomeView(),
            SettingsView()
          ],
          
        ),
      ),
    );
  }
}