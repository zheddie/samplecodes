FROM node:9.4.0-alpine
COPY jstest.js .
RUN npm install && apk update && apk upgrade
CMD node jstest.js
