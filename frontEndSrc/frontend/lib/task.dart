class Task {
  final String id;
  final String plantName;
  final String type;
  final String status;

  Task({
    required this.id,
    required this.plantName,
    required this.type,
    required this.status,
  });

  factory Task.fromJson(Map<String, dynamic> json) {
    return Task(
      id: json['_id'],
      plantName: json['plantName'],
      type: json['type'],
      status: json['status'] ?? 'pending',
    );
  }
}
