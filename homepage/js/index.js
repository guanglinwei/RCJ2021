import * as React from "react";
import * as ReactDOM from "react-dom";
import { Container, Row, Col, Jumbotron } from "react-bootstrap";

const Homepage = (
<main>
    <Container>
        <Jumbotron className="text-center mt-3">
            <h1>Spongebot RCJ 2021</h1>
        </Jumbotron>
        <Row>
            <p>content</p>
            <a href="html/">Docs</a>
        </Row>
    </Container>
</main>
);

// Render under root
ReactDOM.render(Homepage, document.getElementById("root"));