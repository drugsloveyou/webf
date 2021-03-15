xdescribe('windowisglobal', () => {
  it('window equal to globalThis', () => {
    expect(window).toBe(globalThis as any);
  });

  it('has kraken defined', () => {
    // @ts-ignore
    expect(typeof window.kraken).toBe('object');
  });
  it('equal to this', () => {
    function f() {
      // @ts-ignore
      expect(this).toBe(window);
    }

    f();
  });

  it('can set property', () => {
    // @ts-ignore
    window.foo = 'foo';
    // @ts-ignore
    expect(window.foo).toBe('foo');
  });

  it('onload should in window', () => {
    expect('onload' in window).toBe(true);
  });
});