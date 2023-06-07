from fastapi import FastAPI, Response
from fastapi.responses import HTMLResponse
from pydantic import BaseModel
import pandas as pd
import os

app = FastAPI()

csv_file = "sensor_data.csv"


class SensorData(BaseModel):
    lux: float
    ambient_temperature: float
    humidity: float
    pressure: float
    infrared_temperature: float

# Define a function to scan for BLE devices
async def scan():
    devices = []
    scanner = btle.Scanner()
    for device in scanner.scan(5):
        devices.append(device)
    return devices

# Define a route to get a list of BLE devices
@app.get("/scan")
async def get_devices():
    return await scan()

@app.post("/sensor_data/")
async def receive_sensor_data(data: SensorData):
    # Create a new DataFrame with received data
    new_data = pd.DataFrame([{
        "lux": data.lux,
        "ambient_temperature": data.ambient_temperature,
        "humidity": data.humidity,
        "pressure": data.pressure,
        "infrared_temperature": data.infrared_temperature,
    }])

    if os.path.isfile(csv_file):
        # Append new data to the existing CSV file
        with open(csv_file, "a") as f:
            new_data.to_csv(f, header=False, index=False)
    else:
        # Create a new CSV file with the header and data
        new_data.to_csv(csv_file, header=True, index=False)

    return {"message": "Data received successfully"}


@app.get("/data/")
async def get_data():
    if os.path.isfile(csv_file):
        data = pd.read_csv(csv_file)
        return data.to_dict(orient="records")
    else:
        return {"message": "No data available"}


@app.get("/data/html/", response_class=HTMLResponse)
async def get_data_html():
    if os.path.isfile(csv_file):
        data = pd.read_csv(csv_file)
        html_table = data.to_html(index=False, table_id="sensor-data-table")

        return f"""
        <html>
        <head>
            <style>
                table#sensor-data-table {{
                    font-family: Arial, sans-serif;
                    border-collapse: collapse;
                    width: 100%;
                }}
                table#sensor-data-table td, table#sensor-data-table th {{
                    border: 1px solid #ddd;
                    padding: 8px;
                }}
                table#sensor-data-table tr:nth-child(even) {{
                    background-color: #f2f2f2;
                }}
                table#sensor-data-table tr:hover {{
                    background-color: #ddd;
                }}
                table#sensor-data-table th {{
                    padding-top: 12px;
                    padding-bottom: 12px;
                    text-align: left;
                    background-color: #4CAF50;
                    color: white;
                }}
            </style>
        </head>
        <body>
            {html_table}
        </body>
        </html>
        """
    else:
        return "<h1>No data available</h1>"

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
