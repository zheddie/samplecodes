FROM python:3

WORKDIR /usr/src/app

COPY requirements.txt ./
#RUN pip install --no-cache-dir pysocks5
#RUN pip install --no-cache-dir -r requirements.txt --proxy socks5:localhost:7070 --trusted-host pypi.python.org
RUN pip install --no-cache-dir numpy --trusted-host pypi.python.org

COPY . .

CMD [ "python", "./hellonp.py" ]
