#ifndef IMU_H
#define IMU_H

#include "qglobal.h"
#include <QObject>

#define SENSOR_DATA_WIDTH   2
#define SENSOR_AXIS         6
#define SENSOR_BATCH_SIZE   64

typedef struct __attribute__((packed)) bmi160_sensor_data_accel
{
    /*! X-axis sensor data */
    qint16 x;

    /*! Y-axis sensor data */
    qint16 y;

    /*! Z-axis sensor data */
    qint16 z;

    quint32 timestamp;

}imu_sensor_accel_t;

typedef struct __attribute__((packed)) bmi160_sensor_data_gyro
{
    /*! X-axis sensor data */
    qint16 x;

    /*! Y-axis sensor data */
    qint16 y;

    /*! Z-axis sensor data */
    qint16 z;

    quint32 timestamp;

}imu_sensor_gyro_t;

typedef struct __attribute__((packed)) sensor_data
{
    imu_sensor_accel_t accel;
    imu_sensor_gyro_t gyro;
}imu_sensor_data_t;

class IMU : public QObject
{
    Q_OBJECT
public:
    explicit IMU(QObject *parent = nullptr);

signals:

};

#endif // IMU_H
