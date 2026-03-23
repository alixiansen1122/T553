#ifndef BAIDU_MAP_LOCATION_H
#define BAIDU_MAP_LOCATION_H

struct JSCoordinate {
   double lon;
   double lat;
};

/**
   * @brief 将 WGS84 坐标转换为 GCJ02 坐标
   *
   * 将给定的 WGS84 坐标转换为 GCJ02 坐标，并返回转换后的 GCJ02 坐标。
   *
   * @param lon WGS84 经度坐标
   * @param lat WGS84 纬度坐标
   *
   * @return GCJ02 坐标点
   */
extern void WGS84ToGCJ02(double lon, double lat, JSCoordinate &output);

 /**
   * @brief 将 GCJ02 坐标转换为 BD09 坐标
   *
   * 将给定的 GCJ02 坐标转换为 BD09 坐标，并返回转换后的 BD09 坐标。
   *
   * @param lon GCJ02 坐标的经度
   * @param lat GCJ02 坐标的纬度
   *
   * @return 转换后的 BD09 坐标
   */
extern void GCJ02ToBD09(double lon, double lat, JSCoordinate &output);

/**
   * @brief 将WGS84坐标转换为BD09坐标
   *
   * 将给定的 WGS84 坐标转换为 BD09 坐标，并返回转换后的 BD09 坐标。
   *
   * @param lon WGS84经度坐标
   * @param lat WGS84纬度坐标
   *
   * @return 转换后的BD09坐标点
   */
extern void WGS84ToBD09(double lon, double lat, JSCoordinate &output);

/**
   * @brief 将 BD09 坐标转换为 GCJ02 坐标
   *
   * 将给定的 BD09 坐标转换为 GCJ02 坐标，并返回转换后的 GCJ02 坐标。
   *
   * @param bd_lon BD09 经度
   * @param bd_lat BD09 纬度
   *
   * @return 转换后的 GCJ02 坐标
   */
extern void BD09ToGCJ02(double bd_lon, double bd_lat, JSCoordinate &output);
#endif