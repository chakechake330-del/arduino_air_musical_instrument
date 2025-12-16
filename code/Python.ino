import serial
import mido

# ==== 環境に応じて設定してください ====
SERIAL_PORT = 'ここにシリアルポート名を入力（例: /dev/tty.usbmodemXXXX）'
MIDI_PORT_NAME = 'ここにMIDIポート名を入力（例: IAC Driver バス 1）'
# =====================================

BAUD_RATE = 115200

# シリアルポートに接続
ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
print(f'Connected to {SERIAL_PORT} at {BAUD_RATE} baud.')

# MIDI出力ポートを開く
midi_out = mido.open_output(MIDI_PORT_NAME)
print(f'Sending MIDI to {MIDI_PORT_NAME}')

# シリアルからのデータを読み取り、MIDIメッセージを送信
while True:
    try:
        line = ser.readline().decode().strip()
        print(f'Received: {line}')

        if line == 'PAD1_ON':
            midi_out.send(mido.Message('note_on', note=60, velocity=100))
        elif line == 'PAD1_OFF':
            midi_out.send(mido.Message('note_off', note=60, velocity=0))

        # 必要に応じて他のパッドやノート番号も追加可能

    except KeyboardInterrupt:
        print("終了します")
        break
    except Exception as e:
        print(f'エラー: {e}')
