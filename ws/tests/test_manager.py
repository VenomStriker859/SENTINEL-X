"""Dependency-free tests (stdlib only): run with `python -m unittest`."""
import asyncio
import unittest

from ws.manager import ConnectionManager


class FakeWS:
    def __init__(self, fail=False, hang=False):
        self.sent, self.accepted, self.closed = [], False, False
        self.fail, self.hang = fail, hang

    async def accept(self):
        self.accepted = True

    async def send_json(self, payload):
        if self.fail:
            raise RuntimeError("socket closed")
        if self.hang:
            await asyncio.sleep(60)
        self.sent.append(payload)

    async def close(self):
        self.closed = True


class ManagerTests(unittest.IsolatedAsyncioTestCase):
    async def test_connect_sends_welcome_and_tracks(self):
        m, ws = ConnectionManager(), FakeWS()
        await m.connect(ws)
        self.assertTrue(ws.accepted)
        self.assertEqual(m.active_count, 1)
        self.assertEqual(ws.sent[0]["event"], "connected")

    async def test_broadcast_reaches_all_clients(self):
        m = ConnectionManager()
        clients = [FakeWS() for _ in range(3)]
        for c in clients:
            await m.connect(c)
        n = await m.broadcast({"event": "new_message", "data": {"text": "hi"}})
        self.assertEqual(n, 3)
        for c in clients:
            last = c.sent[-1]
            self.assertEqual(last["event"], "new_message")
            self.assertEqual(last["data"], {"text": "hi"})
            self.assertIn("timestamp", last)

    async def test_type_key_alias(self):
        m, ws = ConnectionManager(), FakeWS()
        await m.connect(ws)
        await m.broadcast({"type": "alert", "data": {"level": "high"}})
        self.assertEqual(ws.sent[-1]["event"], "alert")

    async def test_failed_send_drops_only_bad_client(self):
        m = ConnectionManager()
        good, bad = FakeWS(), FakeWS()
        await m.connect(good)
        await m.connect(bad)
        bad.fail = True
        n = await m.broadcast({"event": "alert", "data": {}})
        self.assertEqual(n, 1)
        self.assertEqual(m.active_count, 1)
        self.assertTrue(bad.closed)
        self.assertEqual(good.sent[-1]["event"], "alert")

    async def test_slow_client_times_out(self):
        m = ConnectionManager(send_timeout=0.05)
        good, slow = FakeWS(), FakeWS()
        await m.connect(good)
        await m.connect(slow)
        slow.hang = True
        n = await asyncio.wait_for(m.broadcast({"event": "alert", "data": {}}), 2)
        self.assertEqual(n, 1)
        self.assertEqual(m.active_count, 1)

    async def test_disconnect_is_idempotent(self):
        m, ws = ConnectionManager(), FakeWS()
        await m.connect(ws)
        m.disconnect(ws)
        m.disconnect(ws)
        self.assertEqual(m.active_count, 0)

    async def test_broadcast_with_no_clients(self):
        self.assertEqual(await ConnectionManager().broadcast({"event": "alert"}), 0)

    async def test_malformed_event_rejected(self):
        m = ConnectionManager()
        with self.assertRaises(ValueError):
            await m.broadcast({"data": {}})
        with self.assertRaises(TypeError):
            await m.broadcast("alert")


if __name__ == "__main__":
    unittest.main()
