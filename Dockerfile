FROM gcc:latest

WORKDIR /app
COPY . .

RUN g++ -o external_sort main.cpp external_sort.cpp -std=c++17

CMD ["./external_sort"] 