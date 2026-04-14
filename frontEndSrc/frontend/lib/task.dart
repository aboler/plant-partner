class Task {
  final String id;
  final String type;
  final String status;
  final String? startDate;
  final String? endDate;
  final String? time;
  final List<String> repeatDays;

  Task({
    required this.id,
    required this.type,
    required this.status,
    this.startDate,
    this.endDate,
    this.time,
    required this.repeatDays,
  });

  factory Task.fromJson(Map<String, dynamic> json) {
    return Task(
      id: json['_id'],
      type: json['type'],
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