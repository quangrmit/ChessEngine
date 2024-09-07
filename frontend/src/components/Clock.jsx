import { useEffect, useState } from "react";
import { useTimer } from "react-timer-hook";

function formatTime(number) {
    if (number < 10) {
        return "0" + number.toString();
    } else {
        return number.toString();
    }
}

function Clock({ isWhite, ticking }) {
    let numberColor = isWhite ? "black" : "white";

    let clockMainBackground = isWhite ? "white" : "black";

    let clockSubBackground = isWhite ? "#a3a0a0" : "#615f5f";

    let clockStyle = {
        color: numberColor,
        backgroundColor: ticking ? clockMainBackground : clockSubBackground,
    };

    const time = new Date();
    time.setSeconds(time.getSeconds() + 600); // 10 minutes timer

    const expiryTimestamp = time;

    const { totalSeconds, seconds, minutes, hours, days, isRunning, start, pause, resume, restart } =
        useTimer({ expiryTimestamp, onExpire: () => console.warn("onExpire called") });

    useEffect(() => {
        if (!ticking) {
            pause();
        } else {
            resume();
        }
    }, [ticking]);

    return (
        <div className="clock-container">
            <div className="clock" style={clockStyle}>
                <div>
                    <span>{formatTime(30)}</span>:<span>{formatTime(seconds)}</span>
                </div>
            </div>
        </div>
    );
}

export default Clock;
