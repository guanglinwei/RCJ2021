import * as React from "react";
import * as ReactDOM from "react-dom";
import ReactMarkdown from "react-markdown";
import gfm from "remark-gfm";
import breaks from "remark-breaks";
import { readFileSync } from "fs";
import { Container, Row, Col, Jumbotron, Carousel } from "react-bootstrap";
// Look into https://github.com/remarkjs/remark-directive

import imagesDir from "../img/*.jpg";
// console.log(imagesDir);

// import markdown from "./index.md";
const markdown = readFileSync(__dirname + "/index.md", "utf-8");

const components = {
    // type: (props) => {}
    code: (props) => {
        // className already declared????
        const match = /language-(\w+)/.exec(props.className || '');

        if(!props.inline && match && match[1] === "gallery") {
            // render image gallery

            // image1.png, width, height, caption/alt text
            // image2.png,width,height,"this is the alt text, hello world"

            const images = props.children[0].split("\n") // split lines
                .filter(v => v)
                .map(v => { // for each line
                    const p = v.trim() // remove leading/trailing whitespace
                        .match(/"[^"]*"|[^,]+/g) // split by commas except inside quotes. Ex: a, "b1, b2", c => ["a", ""b1, b2"", "c"]
                        .map(s => s.replace("\"", "")); // remove quotes
                    
                    return {
                        src: imagesDir[p[0]] || "",
                        width: p[1] || 128,
                        height: p[2] || 128,
                        alt: p[3] || "image",
                    };
                });
            
            return (
                <Container fluid className="text-center">
                    <Jumbotron fluid>
                        <Carousel>
                            {images.map((imgProps, i) => {
                            return (
                                <Carousel.Item key={i}>
                                    <img {...imgProps}/>
                                    {(imgProps.alt && imgProps.alt !== "image") &&
                                        <Carousel.Caption className="">
                                            <p className="highlight-black mx-auto w-50 py-2 mt-2">{imgProps.alt}</p>
                                        </Carousel.Caption>
                                    }
                                </Carousel.Item>
                            );
                            })}
                        </Carousel>
                    </Jumbotron>
                </Container>
            );
        }

        const className = language && `language-${language}`;
        const code = React.createElement("code", className ? { className: className } : null, value);
        return React.createElement("pre", {}, code);
    },

    em: ({ node }) => {
        const value = node.children[0].value;
        // console.log(value);
        if(value.includes("LINEBREAK")) return <br/>

        return (
        // <div>
        //     <Container className="border-bottom border-secondary">
        //         <Jumbotron><h2>{value}</h2></Jumbotron>
        //     </Container>
        //     <br/>
        // </div>
        <i>{value}</i>
        );
    },
};

const Homepage = (
    <main>
        <Container className="text-center">
            <ReactMarkdown remarkPlugins={[gfm, breaks]} children={markdown} components={components}/>
        </Container>
    </main>
);

// Render under root
ReactDOM.render(Homepage, document.getElementById("root"));