import { useCallback, useEffect, useState } from "react";

export const useCenteredTree = () => {
    const [translate, setTranslate] = useState({ x: 0, y: 0 });
    const containerRef = useCallback((containerElem) => {
        if (containerElem !== null) {
            const { width, height } = containerElem.getBoundingClientRect();
            setTranslate({ x: width / 2, y: height / 2 });
        }
    }, []);
    return [translate, containerRef];
};

import React from "react";
import Tree from "react-d3-tree";
import { Chessboard } from "react-chessboard";

// import { useCenteredTree } from "./helpers";
// import "./styles.css";

const containerStyles = {
    width: "100vw",
    height: "100vh",
};
const positions = {
    fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    children: [
        {
            fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        },
        {
            fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        },
        {
            fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        },
    ],
};

// Here we're using `renderCustomNodeElement` render a component that uses
// both SVG and HTML tags side-by-side.
// This is made possible by `foreignObject`, which wraps the HTML tags to
// allow for them to be injected into the SVG namespace.
const renderForeignObjectNode = ({ nodeDatum, toggleNode, foreignObjectProps }) => {
    console.log(nodeDatum);
    return (
        <g>
            <circle r={15}></circle>
            {/* `foreignObject` requires width & height to be explicitly set. */}
            <foreignObject {...foreignObjectProps}>
                <div style={{ width: "100px" }}>
                    <Chessboard position={nodeDatum.fen} />

                    {nodeDatum.children && (
                        <button style={{ width: "100%" }} onClick={toggleNode}>
                            {nodeDatum.__rd3t.collapsed ? "Expand" : "Collapse"}
                        </button>
                    )}
                </div>
            </foreignObject>
        </g>
    );
};

export default function VisualizeTree() {
    const [translate, containerRef] = useCenteredTree();
    const nodeSize = { x: 200, y: 200 };
    const foreignObjectProps = { width: nodeSize.x, height: nodeSize.y, x: 20 };

    const [positions, setPositions] = useState({});

    const getData = async () => {
        let data = await fetch("http://127.0.0.1:5501/data");
        let test = await data.json();
        console.log(test);
        setPositions(test);
        return test;
    };
    useEffect(() => {
        getData();
    }, []);
    return (
        <div style={containerStyles} ref={containerRef}>
            <Tree
                data={positions}
                translate={translate}
                nodeSize={nodeSize}
                renderCustomNodeElement={(rd3tProps) =>
                    renderForeignObjectNode({ ...rd3tProps, foreignObjectProps })
                }
                orientation="vertical"
            />
        </div>
    );
}
