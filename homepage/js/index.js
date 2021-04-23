import * as React from "react";
import * as ReactDOM from "react-dom";
import ReactMarkdown from "react-markdown";
import gfm from "remark-gfm";
import breaks from "remark-breaks";
import { Container, Row, Col, Jumbotron } from "react-bootstrap";
// Look into https://github.com/remarkjs/remark-directive

import markdown from "./index.md";

const components = {
    // type: (props) => {}
};

const Homepage = (
    <main>
        <ReactMarkdown remarkPlugins={[gfm, breaks]} children={markdown} components={components}/>
    </main>
);

// const Homepage = (
// <main>
//     <Container>
//         <Jumbotron className="text-center mt-3">
//             <h1>Spongebot RCJ 2021</h1>
//         </Jumbotron>
//         <Row>
//             <p>content</p>
//             <br/>
//             <p>
//                 <a href="html/">Docs</a>
//             </p>
//         </Row>
//     </Container>
// </main>
// );

// Render under root
ReactDOM.render(Homepage, document.getElementById("root"));