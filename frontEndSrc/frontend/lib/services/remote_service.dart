import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:frontend/plant.dart';
import 'package:frontend/task.dart';
import 'dart:convert';

//const String baseUrl = 'http://10.###.###.###:8000/plants/getPlantByName/Sunflower';
const String baseUrl = 'http://10.0.2.2:8000/plants/getPlantByName/Sunflower';
//const String baseUrl = 'http://172.20.10.7:8000/plants/getPlantByName/Sunflower';

class RemoteService {
  static const String url = "http://10.0.2.2:8000";

  Future<Plant?> getPlant() async {
    final resp = await http.get(Uri.parse(baseUrl));

    if (resp.statusCode == 200) {
      print('made it thru');
      final List data = json.decode(resp.body);

      if (data.isNotEmpty) {
        return Plant.fromJson(data[0]); // since list contains one plant
      }
    } else {
      print('failed: ${resp.statusCode}');
    }

    return null;
  }

  Future<bool> setAutoSchedule(bool enabled) async {
    final resp = await http.put(
      Uri.parse("$url/plants/updateAutoSchedule/Sunflower"),
      headers: {'Content-Type': 'application/json'},
      body: jsonEncode({'autoSchedule': enabled}),
    );

    return resp.statusCode == 200;
  }

  // gets all tasks for the plant
  Future<List<Task>> getTasks() async {
    final resp = await http.get(
      Uri.parse("$url/tasks/getTasks/Sunflower"),
    );

    if (resp.statusCode == 200) {
      final List data = json.decode(resp.body);
      return data.map((e) => Task.fromJson(e)).toList();
    } else {
      print('failed getTasks: ${resp.statusCode}');
      return [];
    }
  }

  // creates a new task (start with type="water")
  Future<bool> createTask(String type) async {
    final resp = await http.post(
      Uri.parse("$url/tasks/createTask"),
      headers: {'Content-Type': 'application/json'},
      body: jsonEncode({
        'plantName': 'Sunflower',
        'type': type,
        'status': 'pending',
      }),
    );

    if (resp.statusCode != 200) {
      print('failed createTask: ${resp.statusCode}');
    }

    return resp.statusCode == 200;
  }

  // marks a task as done
  Future<bool> markTaskDone(String id) async {
    final resp = await http.put(
      Uri.parse("$url/tasks/markDone/$id"),
    );

    if (resp.statusCode != 200) {
      print('failed markTaskDone: ${resp.statusCode}');
    }

    return resp.statusCode == 200;
  }
}
