class Plant {
  final String plantName;
  final double soilMoisture;
  final int lightIntensity;
  final int nLevel;
  final int pLevel;
  final int kLevel;

  Plant({
    required this.plantName,
    required this.soilMoisture,
    required this.lightIntensity,
    required this.nLevel,
    required this.pLevel,
    required this.kLevel,
  });

  factory Plant.fromJson(Map<String, dynamic> json) {
    return Plant(
      plantName: json['plantName'],
      soilMoisture: (json['soilMoisture'] as num).toDouble(),
      lightIntensity: json['lightIntensity'],
      nLevel: json['nLevel'],
      pLevel: json['pLevel'],
      kLevel: json['kLevel'],
    );
  }
}