function func(arg) {
  return `Hello ${arg} from ${process.pid}`;
}

process.on('SIGUSR2', () => {
  process.exit(1);
});

process.on('message', (arg) => {
  if (arg === 'Vova') {
    process.exit(1);
  }
  const result =  func(arg);
  process.send(result);
});

