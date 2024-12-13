from prophet import Prophet
import pandas as pd
import sys

[_, path, output, freq, periods] = sys.argv

print(path, output, freq, periods)

# path = 'C:/Users/Alessandro/Downloads/Cartel1.csv'

df = pd.read_csv(path, delimiter=',', parse_dates=['ds'], date_format='%d/%m/%Y')
print('read')
print(df.head())

m = Prophet()
m.fit(df)

future = m.make_future_dataframe(freq=freq, periods=int(periods))
print('future')
print(future.tail())

forecast = m.predict(future)

print("Forecast")
print("Forecast number " + str(len(forecast)))
print(forecast.tail())

forecast.to_csv(output)
