class Task {
  final String id;
  final List<String> taskTypes;
  final String status;
  final String? startDate;
  final String? endDate;
  final String? time;
  final List<String> repeatDays;

  Task({
    required this.id,
    required this.taskTypes,
    required this.status,
    this.startDate,
    this.endDate,
    this.time,
    required this.repeatDays,
  });

  factory Task.fromJson(Map<String, dynamic> json) {
    List<String> parsedTaskTypes = [];

    if (json['taskTypes'] != null) {
      parsedTaskTypes = List<String>.from(json['taskTypes']);
    } else if (json['type'] != null) {
      parsedTaskTypes = [json['type']];
    }

    return Task(
      id: json['_id'],
      taskTypes: parsedTaskTypes,
      status: json['status'] ?? '',
      startDate: json['startDate'],
      endDate: json['endDate'],
      time: json['time'],
      repeatDays: json['repeatDays'] != null
          ? List<String>.from(json['repeatDays'])
          : [],
    );
  }
}