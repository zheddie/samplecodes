FROM adoptopenjdk:11-jre-openj9
RUN mkdir /opt/app
RUN apt-get update && apt-get install -y libxext6 libxrender1 libxtst6 libxi6
COPY acsbundle.jar /opt/app
COPY start.sh /opt/app
CMD ["sh", "/opt/app/start.sh"]
