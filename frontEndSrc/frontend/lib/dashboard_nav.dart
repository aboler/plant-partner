import 'package:flutter/material.dart';
import 'package:frontend/navPages/dashView.dart';
import 'package:frontend/navPages/taskView.dart';

class Dashboard extends StatefulWidget {
  const Dashboard({super.key});

  @override
  _DashboardState createState() => _DashboardState();
}

class _DashboardState extends State<Dashboard> {
  GlobalKey<NavigatorState> dashNavKey = GlobalKey<NavigatorState>();

  @override
  Widget build(BuildContext context) {
    return Navigator(
      key: dashNavKey,
      onGenerateRoute: (RouteSettings settings) {
        return MaterialPageRoute(
          settings: settings,
          builder: (BuildContext context) {
            if (settings.name == '') {
              //details page, container temp
              return const DashView();
            }

            //main page
            return const TaskView();
          },
        );
      },
    );
  }
}
