using System;
using System.IO;
using System.Linq;

namespace GCIPLDec
{
    static class Program
    {
        internal static bool b(this int i) => Convert.ToBoolean(i);

        // Implementation based on https://github.com/ogamespec/gc-ipl/blob/wiki/Descrambler.md
        private static void Descramble(byte[] Data)
        {
            byte acc = 0, nacc = 0, x = 1;
            ushort t = 0x2953, u = 0xd9c2, v = 0x3ff1;

            for (int it = 0; it < Data.Length;)
            {
                int t0 = t & 1, t1 = (t >> 1) & 1, u0 = u & 1, u1 = (u >> 1) & 1, v0 = v & 1;

                x ^= (byte)(t1 ^ v0);
                x ^= (byte)(u0 | u1);
                x ^= (byte)((t0 ^ u1 ^ v0) & (t0 ^ u0));

                if (t0 == u0)
                {
                    v >>= 1;
                    if (v0.b()) v ^= 0xb3d0;
                }

                if (t0 == 0)
                {
                    u >>= 1;
                    if (u0.b()) u ^= 0xfb10;
                }

                t >>= 1;

                if (t0.b()) t ^= 0xa740;

                nacc++;

                acc = (byte)((acc << 1) + x);

                if (nacc == 8)
                {
                    Data[it++] ^= acc;
                    nacc = 0;
                }
            }
        }

        private static void Main(string[] args)
        {
            var In = File.ReadAllBytes(args[0]).Skip(0x100).ToArray();
            Descramble(In);
            File.WriteAllBytes($"{args[0]}.decrypted", In);
        }
    }
}