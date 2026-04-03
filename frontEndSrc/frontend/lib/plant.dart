class Plant {
  final String plantName;
  final double soilMoisture;
  final int lightIntensity;
  final int nLevel;
  final int pLevel;
  final int kLevel;
  final bool autoSchedule;

  Plant({
    required this.plantName,
    required this.soilMoisture,
    required this.lightIntensity,
    required this.nLevel,
    required this.pLevel,
    required this.kLevel,
    required this.autoSchedule,
  });

  factory Plant.fromJson(Map<String, dynamic> json) {
    return Plant(
      plantName: json['plantName'],
      soilMoisture: (json['soilMoisture'] as num).toDouble(),
      lightIntensity: json['lightIntensity'],
      nLevel: json['nLevel'],
      pLevel: json['pLevel'],
      kLevel: json['kLevel'],
      autoSchedule: json['autoSchedule'] ?? false,
    );
  }

  static String moistureLevel(double value) {
    if (value < 1500) return "High"; //wet
    if (value < 1900) return "Average";
    return "Low"; //dry [1900, 2803 max]
  }

  static String lightLevel(int value) {
    if (value < 1000) return "Low"; //dark
    if (value <= 1400) return "Average";
    return "High"; //sunny
  }

  static String nutrientLevel(int value) {
    if (value < 6) return "Low";
    if (value < 21) return "Average";
    return "High";
  }
  //need the threshold to be that whenthe sum off these nutrients <60
  // then turn on the fertilizer
}