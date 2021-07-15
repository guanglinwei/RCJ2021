import * as React from "react";
import * as ReactDOM from "react-dom";
import ReactMarkdown from "react-markdown";
import gfm from "remark-gfm";
import breaks from "remark-breaks";
import { readFileSync } from "fs";
import { Container, Row, Col, Jumbotron, Carousel } from "react-bootstrap";
// Look into https://github.com/remarkjs/remark-directive

/**
 * If you want to add images, put them in ../img/
 * 
 * If you want to add text, put it in ./index.md
 * 
 * 
 * Image galleries are in code blocks.
 * 
 * ```gallery
 * {OPTIONS}
 * IMAGE 1
 * IMAGE 2
 * ```
 * 
 * Read ./example.md for details
 * Do not forget file extensions
 * 
 * If you want to add an image on its own, use !(alt text)[image]
 * NOTE: the alt text can be used to set the dimensions of the image. The format is as follows:
 * alt text, width, height
 * Ex: !(A robot, 300, 300)[robot.png] -> results in a 300x300 robot.png with the alt text "A robot"
 * 
 * To view the webpage, cd in the homepage folder and use "npm run dev"
 * (Make sure NPM is installed)
 * 
 * When you're done use "npm run build"
 * The github page will update on its own.
 */


// import markdown from "./index.md";
const markdown = readFileSync(__dirname + "/index.md", "utf-8");

const components = {
    // fix bug where images were treated a paragraphs
    p: (props) => {
        const element = props.children[0];
        const {node, ...pProps} = props;
        return (element.type.name === 'img') ? { ...element } : <p {...pProps} />;
    },

    a: (props) => {
        const value = props.children[0];
        // begin with '#' to not open in a new tab (for relative links)
        if(value.charAt(0) == '#') {
            props.children[0] = props.children[0].slice(1, value.length);
            return <a href={props.href}>{props.children}</a>
        }
        return <a href={props.href} target="_blank" rel="noreferrer nofollow noopener">{props.children}</a>
        // return <a>a</a>
    },

    img: ({ src, alt }) => {
        // use alt for width and height
        // alt, width, height
        
        const [a, w, h] = alt.split(/\,\s?/);
        return (
            <div>
                <img src={src} alt={a} width={w || 300} height={h || 300}/>
                <br/>
            </div>
        );
    },

    // type: (props) => {}
    code: (props) => {
        // className already declared????
        const match = /language-(\w+)/.exec(props.className || '');

        if(!props.inline && match && match[1] === "gallery") {
            // render image gallery

            // image1.png, width, height, caption/alt text
            // image2.png,width,height,"this is the alt text, hello world"
            const spl = props.children[0].split("\n");

            // options for gallery must be on first line
            // {option 1, option 2, ...}
            let galleryOptions = undefined;

            if(spl[0] && spl[0].charAt(0) === '{') {
                galleryOptions = JSON.parse(spl[0]);
                // galleryOptions = spl[0].replace(/\{|\}/, '') // remove { }
                //     .split(/,\s*/) // split by comma and >=0 spaces
                //     .map(v => v.toLowerCase()); // all lowercase chars
            }

            const images = spl // split lines
                .filter((v, i) => v && !(galleryOptions && i === 0)) // remove empty lines and first line
                .map(v => { // for each line
                    const p = v.trim() // remove leading/trailing whitespace
                        .match(/"[^"]*"|[^,\s][^\,]*[^,\s]*/g) // split by commas except inside quotes. Ex: a, "b1, b2", c => ["a", ""b1, b2"", "c"] /"[^"]*"|[^,]+/g
                        .map(s => s.replace("\"", "")); // remove quotes
            
                    return {
                        // src: imagesDir[p[0]] || "",
                        src: p[0] || "",
                        width: p[1] || 128,
                        height: p[2] || 128,
                        alt: p[3] || "image",
                    };
                });

            if(galleryOptions) {
                switch(galleryOptions.type) {
                    case "table":
                        const tbW = galleryOptions.width || 2;
                        const tbH = galleryOptions.height || 2;

                        const contents = [];
                        for(let i = 0; i < tbH; i++) {
                            const inner = [];
                            for(let j = 0; j < tbW; j++) {
                                if(i * tbW + j >= images.length) break;
                                inner.push(<Col key={j} className="align-items-center d-flex"><img className="img-fluid" {...(images[i * tbW + j])}/></Col>)
                            }
                            contents.push(<Row key={i}>{inner}</Row>);
                        }
                        return (
                            <Container fluid className="text-center">
                                {contents}
                            </Container>
                        );

                    // default: Bootstrap Carousel
                    default:            
                        break;
                }
            }
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