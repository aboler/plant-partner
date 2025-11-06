import 'package:flutter/material.dart';
//import 'package:frontend/dashboard_nav.dart';
import 'package:frontend/navPages/homeView.dart';
import 'package:frontend/navPages/settingsView.dart';
import 'package:frontend/navPages/taskView.dart';

class Nav extends StatefulWidget {
  const Nav({super.key});

  @override
  State<Nav> createState() => _NavState();
}

class _NavState extends State<Nav> {
  //int selIndex = 0;
  // List<Widget> widOpt = <Widget>[
  //   Text('Tasks'),
  //   Text('Home'),
  //   Text('Settings'),

  // ];

  // void onItemTap(int index) {
  //   setState(() {
  //     selIndex = index;
  //   });
  // }
  int _selectedIndex = 0;

  @override
  Widget build(BuildContext context) {
    // return Scaffold(
    //   //body: Center(child: widOpt.elementAt(selIndex),),
    //   //body: ListView.builder(itemCount: widOpt.length, itemBuilder: (context, widOptIndex) {Widget widd = widOpt[widOptIndex]},),
    //   //navbar is a list 0-2
    //   bottomNavigationBar: BottomNavigationBar(
    //     items: const <BottomNavigationBarItem>[
    //       BottomNavigationBarItem(
    //         icon: Icon(Icons.calendar_month_rounded),
    //         label: 'Tasks',
    //       ),
    //       BottomNavigationBarItem(icon: Icon(Icons.home), label: 'Home'),
    //       BottomNavigationBarItem(
    //         icon: Icon(Icons.settings),
    //         label: 'Settings',
    //       ),
    //     ],
    //     // currentIndex: selIndex,
    //     // onTap: onItemTap,
    //   ),
    // );

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