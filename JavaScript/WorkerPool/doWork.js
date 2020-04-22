function func(path) {
  return `Server got request for path ${path}, PID handling request is: ${process.pid}`;
}

process.on('message', (path) => {
  const result =  func(path);
  process.send(result);
});